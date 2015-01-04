#include "Game/Physics/MergeBuilder.h"

using namespace Wulf;
using namespace Wulf::Physics;

MergeNode::MergeNode(Map::Node const& node)
	: topLeft(node.x, node.y), type(NodeToTile(node))
	, width(1), height(1)
	, done(false)
	, mTileData(nullptr)
{
}

bool MergeNode::compatible(MergeNode const* other) const
{
	if (other == nullptr || other->done || other->type != type)
		return false;
	return true;
}

// This returns the tiledata for the merged mass.
// It is safe to call multiple times as it only returns one TileData.
TileData* MergeNode::toTile()
{
	if (mTileData != nullptr)
		return mTileData;
	// Because short + short coerces to int if left to itself.
	coord brx = topLeft.x + width, bry = topLeft.y + height;
	mTileData = new TileData(topLeft, coords(brx, bry), type);
	return mTileData;
}

MergeBuilder::MergeBuilder(Map::Map const& map)
{
	for (auto& xnodes : nodes)
		std::fill(xnodes.begin(), xnodes.end(), nullptr);
	loadMap(map);
	performMerges();
}
MergeBuilder::~MergeBuilder()
{
	for (auto& xnodes : nodes) {
		for (MergeNode* node : xnodes) {
			delete node;
		}
	}
}

inline
void MergeBuilder::loadMap(Map::Map const& map)
{
	const auto& mapNodes = map.nodes;
	for (int x = 0; x < xsize; x++) {
		const auto& xnodes = mapNodes[x];
		for (int y = 0; y < ysize; y++) {
			const auto& node = xnodes[y];
			// Filter out walls the player can never hit
			if (node.wall && !node.visibleWall) {
				// Make sure corners are included
				byte i = 0;
				for (Map::Node *neighbour : node.neighbours) {
					if (neighbour != nullptr && neighbour->visibleWall) {
						i++;
					}
				}
				if (i < 2)
					continue;
			}
			nodes[x][y] = new MergeNode(node);
		}
	}
}

inline
void MergeBuilder::performMerges()
{
	// Vars defined out here for probable performance increase?
	byte i, j, yadd;
	MergeNode *node, *other;
	bool xbroke;
	for (int x = 0; x < xsize; x++) {
		for (int y = 0; y < ysize; y++) {
			node = nodes[x][y];
			if (node == nullptr || node->done)
				continue;
			// Merge vertically first due to how the loop is layed out.
			i = 1;
			while (y + i < ysize) { // Ensure we stay within bounds
				other = nodes[x][y + i];
				// Check if the node is the same kind as us. (includes a nullptr check)
				if (!node->compatible(other))
					break;

				// Eat the node
				node->height++;
				delete other;
				nodes[x][y + i] = node;

				i++;
			}
			// yadd lets us skip consumed nodes for the next iteration of y.
			yadd = i - 1;

			i = 1;
			xbroke = false;
			// Consume horizontally
			while (x + i < xsize) {
				// Make sure that we can consume all x tiles for our height
				for (j = 0; j < node->height; j++) {
					other = nodes[x + i][y + j];
					if (!node->compatible(other)) {
						xbroke = true;
						break;
					}
				}
				if (xbroke)
					break;
				node->width++;
				// Eat all nodes for our height
				for (j = 0; j < node->height; j++) {
					delete nodes[x + i][y + j];
					nodes[x + i][y + j] = node;
				}

				i++;
			}
			node->done = true;

			// Skip the ones we consumed on this column
			y += yadd;
		}
	}
}

std::vector<std::pair<coords, TileData*>> MergeBuilder::getTileData() const
{
	std::vector<std::pair<coords, TileData*>> ret;
	ret.reserve(xsize * ysize);
	const coord xhalf = Map::Map::halfwidth;
	const coord yhalf = Map::Map::halfheight;

	for (coord x = 0; x < xsize; x++) {
		for (coord y = 0; y < ysize; y++) {
			MergeNode* node = nodes[x][y];
			if (node == nullptr)
				continue;
			coord nodeX = -(x - xhalf); // I don't know why this is negative, but it is.
			coord nodeY = y - yhalf;
			ret.emplace_back(coords(nodeX, nodeY), node->toTile());
		}
	}

	return ret;
}
