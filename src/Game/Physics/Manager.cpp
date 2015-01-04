#include <array>
#include <utility>
#include <algorithm>
#include <glm\glm.hpp>
#include "Game/Physics/Manager.h"
#include "Game/Physics/TileData.h"

using namespace Wulf;
using namespace Wulf::Physics;

Manager Manager::instance;

CollisionObj getObj(const Entity& ent)
{
	Vector pos = ent.GetPos();
	float x = pos.x;
	float y = pos.y;
	Vector bounds = ent.GetCollisionBounds();
	float w = bounds.x / 2;
	float h = bounds.y / 2;
	CollisionObj res = {
		// left
		x - w,
		// top
		y - h,
		// right
		x + w,
		// bottom
		y + h
	};
	return res;
}

Manager::~Manager()
{
	for (auto& value : map)
		delete value.second;
}

void Manager::UpdateDoor(const coords pos, const bool state)
{
	auto node = map.find(pos);
	if (node != map.end())
		node->second->Solid = state;
}

Manager::TileSet Manager::grabTiles(const Vector& pos) const
{
	// Distance to center may not be a very useful metric
	TileSet ret([pos](TileData const * a, TileData const * b) -> bool {
		return glm::distance(a->center, pos) < glm::distance(b->center, pos);
	});
	coords const cpos(pos);
	auto const& none = map.end();
	for (coord x = cpos.x - 1; x <= cpos.x + 1; ++x) {
		for (coord y = cpos.y - 1; y <= cpos.y + 1; ++y) {
			auto const& itr = map.find(coords(x, y));
			// Check for out of bounds
			if (itr == none)
				continue;
			TileData const *tile = itr->second;
			if (tile->Solid)
				ret.insert(tile);
		}
	}
	return ret;
}

struct MergeBuilder {
	coords topLeft;
	TileType type;

	coord width, height;
	bool done;

	MergeBuilder(Map::Node const& node)
		: topLeft(node.x, node.y), type(NodeToTile(node))
		, width(1), height(1)
		, done(false)
		, mTileData(nullptr)
	{
	}

	bool compatible(MergeBuilder const* other) const
	{
		if (other == nullptr || other->done || other->type != type)
			return false;
		return true;
	}

	// This returns the tiledata for the merged mass.
	// It is safe to call multiple times as it only returns one TileData.
	TileData* toTile()
	{
		if (mTileData != nullptr)
			return mTileData;
		// Because short + short coerces to int if left to itself.
		coord brx = topLeft.x + width, bry = topLeft.y + height;
		mTileData = new TileData(topLeft, coords(brx, bry), type);
		return mTileData;
	}
private:
	// Note that this should not be deleted - that is handled elsewhere.
	TileData* mTileData;
};

void Manager::SetMap(Map::Map const& m)
{
	const auto& nodes = m.nodes;

	const size_t xsize = Map::Map::width, ysize = Map::Map::height;

	// Initial creation run
	std::array<std::array<MergeBuilder*, ysize>, xsize> setup;
	// nullptrs for everyone
	for (auto& nodes : setup)
		std::fill(nodes.begin(), nodes.end(), nullptr);

	// Insert all viable tiles into a static 64x64 array
	// Leaves unviable (ie wall cavities) as nullptr;
	for (int x = 0; x < xsize; x++) {
		const auto& xnodes = nodes[x];
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
			setup[x][y] = new MergeBuilder(node);
			// If I was massochistic (and this was performance critical), I could do the horizonal merge here.
			// Neither is the case.
		}
	}
	// Vars defined out here for probable performance increase?
	byte i, j, yadd;
	MergeBuilder *node, *other;
	bool xbroke;
	// Go through everything and start merging.
	// This replaces merged tiles with multiple pointers to the same tiledata.
	for (int x = 0; x < xsize; x++) {
		for (int y = 0; y < ysize; y++) {
			node = setup[x][y];
			if (node == nullptr || node->done)
				continue;
			// Merge vertically first due to how the loop is layed out.
			i = 1;
			while (y + i < ysize) { // Ensure we stay within bounds
				other = setup[x][y + i];
				// Check if the node is the same kind as us. (includes a nullptr check)
				if (!node->compatible(other))
					break;

				// Eat the node
				node->height++;
				delete other;
				setup[x][y + i] = node;

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
					other = setup[x + i][y + j];
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
					delete setup[x + i][y + j];
					setup[x + i][y + j] = node;
				}

				i++;
			}
			node->done = true;

			// Skip the ones we consumed on this column
			y += yadd;
		}
	}

	map.clear();
	// I've got no idea how much this really needs, but this seems a saneish value.
	map.reserve(xsize * ysize / 2);
	// Convert the merged data into tiledatas.
	for (coord x = 0; x < xsize; x++) {
		for (coord y = 0; y < ysize; y++) {
			node = setup[x][y];
			if (node == nullptr)
				continue;
			map.insert(std::make_pair(coords(x, y), node->toTile()));
		}
	}
	// Cleanup
	for (auto& x : setup) {
		for (MergeBuilder *y : x) {
			delete y;
		}
	}
}

/*
 * TODO:
 * Instead of selecting the 9 ajacent blocks and merging their faces
 *  consider merging all blocks at the start and then selecting the close ones.
 * This probably needs some kind of spatial management.
 */

Vector Manager::CollisionClamp(const Entity& entity, const Vector& velocity) const
{
	// Uh.
	CollisionObj ent = getObj(entity);

	auto nodes = grabTiles(entity.GetPos());

	// Debuggering
	debugger.UpdateScreen(entity, std::vector<TileData const * const>(nodes.cbegin(), nodes.cend()));

	bool collided = false;

	float x = velocity.x;
	float y = velocity.y;
	do {
		collided = false;
		for (const TileData *ctile : nodes) {
			const CollisionObj& tile = ctile->Bounds;
			// Basic checks
			// Note, this is in GUI space not cartesean
			if (ent.Left + x >= tile.Right)
				continue;
			if (ent.Right + x <= tile.Left)
				continue;
			// positive Y is DOWN.
			if (ent.Bottom + y <= tile.Top)
				continue;
			if (ent.Top + y >= tile.Bottom)
				continue;

			collided = true;

			float xchange = 0;
			float ychange = 0;
			if (x > 0)
				// Collision from the right to the left ( > )
				xchange = tile.Left - (ent.Right + x);
			else
				// Collision from the left to the right ( < )
				xchange = (tile.Right - ent.Left) - x;

			if (y > 0)
				// Collision from above to below ( v )
				ychange = tile.Top - (ent.Bottom + y);
			else
				// Collision from below to above ( ^ )
				ychange = (tile.Bottom - ent.Top) - y;

			// Only punt the smallest distance possible
			if (fabs(ychange) < fabs(xchange))
				y += ychange;
			else
				x += xchange;

			// Reboot the loop with the new velocity
			break;
		}
		// TODO: Anti infinite-loop protection
	} while (collided);

	return Vector(x, y, 0);
}
