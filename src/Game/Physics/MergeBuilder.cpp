#include <set>
#include "Game/Physics/MergeBuilder.h"

#ifdef DEBUG
//#define DEBUG_MAPSPEW
#endif
#ifdef DEBUG_MAPSPEW
#include <iostream>
#include <sstream>
#endif

using namespace Wulf;
using namespace Wulf::Physics;

bool notMergable(TileType type)
{
	return type == TileType::Door || type == TileType::Pickup;
}

MergeNode::MergeNode(Map::Node const& node)
	: topLeft(node.x, node.y), type(NodeToTile(node))
	, width(0), height(0)
	, done(false)
	, mTileData(nullptr)
{
}

bool MergeNode::compatible(MergeNode const* other) const
{
	if (other == nullptr || other->done || other->type != type || notMergable(other->type))
		return false;
	return true;
}

// This returns the tiledata for the merged mass.
// It is safe to call multiple times as it only returns one TileData.
TileData* MergeNode::toTile()
{
	if (mTileData != nullptr)
		return mTileData;
	coords topLeft = this->topLeft;
	int tlx = topLeft.x;
	topLeft.x -= width;
	coords bottomRight = coords(tlx, topLeft.y + height);
	mTileData = new TileData(topLeft, bottomRight, type);
	return mTileData;
}

std::tuple<std::string, std::string, std::string> MergeNode::toString(coords tile) const
{
#ifdef DEBUG_MAPSPEW
	std::string contents;
	if (type == TileType::Empty) {
		contents = ".";
	} else if (type == TileType::Pickup) {
		contents = "x";
	} else if (type == TileType::Sprite) {
		contents = "~";
	} else if (type == TileType::Door) {
		contents = "D";
	} else {
		contents = "#";
	}

	std::stringstream a, b, c;
	coords bottomRight(topLeft.x - width, topLeft.y + height);
	if (tile == topLeft) {
		a << "/-";
		b << "|";
	} else if (tile.x == topLeft.x) {
		a << "|" << contents;
		b << "|";
	} else if (tile.y == topLeft.y) {
		a << "--";
		b << contents;
	} else {
		a << contents << contents;
		b << contents;
	}
	b << contents;
	if (tile.y == topLeft.y) {
		if (tile.x == bottomRight.x) {
			a << "\\";
			b << "|";
		} else {
			a << "-";
			b << contents;
		}
	} else if (tile.x == bottomRight.x) {
		a << "|";
		b << "|";
	} else {
		a << contents;
		b << contents;
	}
	if (tile.x == topLeft.x) {
		if (tile.y == bottomRight.y) {
			c << "\\-";
		} else {
			c << "|" << contents;
		}
	} else if (tile.y == bottomRight.y) {
		c << "--";
	} else {
		c << contents << contents;
	}
	if (tile == bottomRight) {
		c << "/";
	} else if (tile.x == bottomRight.x) {
		c << "|";
	} else if (tile.y == bottomRight.y) {
		c << "-";
	} else {
		c << contents;
	}
	return std::make_tuple(a.str(), b.str(), c.str());
#else
	return std::make_tuple("", "", "");
#endif
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
	// Use a set because we have duplicate pointers
	std::set<MergeNode*> pointrs;
	for (auto& xnodes : nodes) {
		pointrs.insert(xnodes.begin(), xnodes.end());
	}
	for (MergeNode* node : pointrs) {
		delete node;
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
int MergeBuilder::verticalMerge(int x, int y)
{
	MergeNode *node = nodes[x][y];
	int i = 1;
	while (y + i < ysize) { // Ensure we stay within bounds
		MergeNode *other = nodes[x][y + i];
		// Check if the node is the same kind as us. (includes a nullptr check)
		if (!node->compatible(other))
			break;

		// Eat the node
		node->height++;
		delete other;
		nodes[x][y + i] = node;

		i++;
	}
	return i - 1;
}

inline
void MergeBuilder::horizontalMerge(int x, int y)
{
	MergeNode *node = nodes[x][y];
	int i = 1;
	// Consume horizontally
	while (x + i < xsize) {
		// Make sure that we can consume all x tiles for our height
		for (int j = 0; j <= node->height; j++) {
			MergeNode *other = nodes[x + i][y + j];
			if (!node->compatible(other)) {
				return;
			}
		}
		node->width++;
		// Eat all nodes for our height
		for (int j = 0; j <= node->height; j++) {
			delete nodes[x + i][y + j];
			nodes[x + i][y + j] = node;
		}

		i++;
	}
}

inline
void MergeBuilder::performMerges()
{
	for (int x = 0; x < xsize; x++) {
		for (int y = 0; y < ysize; y++) {
			MergeNode *node = nodes[x][y];
			if (node == nullptr || node->done || notMergable(node->type))
				continue;
			// Merge vertically first due to how the loop is layed out.
			int yadd = verticalMerge(x, y);
			horizontalMerge(x, y);
			node->done = true;
			// yadd lets us skip consumed nodes for the next iteration of y.
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

	// SPAM
#ifdef DEBUG_MAPSPEW
	DebugOutput();
#endif

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

void MergeBuilder::DebugOutput() const
{
#ifdef DEBUG_MAPSPEW
	const coord xhalf = Map::Map::halfwidth;
	const coord yhalf = Map::Map::halfheight;

	coord xcoord, ycoord;

	std::cout << " vim: nowrap listchars=" << std::endl;
	std::cout.setf(std::ios_base::right);
	std::cout.fill(' ');
	std::cout.width(3);
	std::cout << "  ";
	std::ostringstream bluh;
	bluh << "   ";
	for (byte x = 0; x < xsize; ++x) {
		std::cout.width(3);
		std::cout << -(x - xhalf);
		bluh << "***";
	}
	std::cout << std::endl;
	std::cout << bluh.str() << std::endl;
	for (byte y = 0; y < ysize; ++y) {
		ycoord = (y - yhalf);
		std::stringstream a, b, c;
		a << "   *";
		b.width(3);
		b << ycoord << '*';
		c << "   *";
		for (byte x = 0; x < xsize; ++x) {
			xcoord = -(x - xhalf);
			MergeNode *node = nodes[x][y];
			if (node == nullptr) {
				a << "   ";
				b << "   ";
				c << "   ";
				continue;
			}
			auto lines = node->toString(coords(xcoord, ycoord));
			a << std::get<0>(lines);
			b << std::get<1>(lines);
			c << std::get<2>(lines);
		}
		std::cout << a.str() << std::endl;
		std::cout << b.str() << std::endl;
		std::cout << c.str() << std::endl;
	}
#endif
}
