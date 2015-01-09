#include <array>
#include <utility>
#include <algorithm>
#include <glm\glm.hpp>
#include "Game/Physics/Manager.h"
#include "Game/Physics/TileData.h"
#include "Game/Physics/MergeBuilder.h"

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
	// Use a set because we have duplicate pointers
	std::set<TileData*> pointrs;
	for (auto& value : map)
		pointrs.insert(value.second);
	for (TileData* node : pointrs)
		delete node;
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
		// Equality check
		if (a == b)
			return false;
		float d1, d2;
		d1 = glm::distance(a->center, pos);
		d2 = glm::distance(b->center, pos);
		if (d1 != d2)
			return d1 < d2;
		// The user is exactly centered between two tiles. (Most likely a corner!)
		// At this point order doesn't matter, so use pointer value.
		return a < b;
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

void Manager::SetMap(Map::Map const& m)
{
	MergeBuilder builder(m);
	auto& newNodes = builder.getTileData();

	map.clear();
	for (auto& node : newNodes) {
		map.insert(node);
	}
}

Vector Manager::CollisionClamp(const Entity& entity, const Vector& velocity) const
{
	// Uh.
	CollisionObj ent = getObj(entity);

	auto nodes = grabTiles(entity.GetPos());

	// Debuggering
	//debugger.UpdateScreen(entity, std::vector<TileData const * const>(nodes.cbegin(), nodes.cend()));

	bool collided = false;

	float x = velocity.x;
	float y = velocity.y;
	// Anti infinite loop
	int i = 0;
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
		// Anti infinite-loop protection
		i++;
	} while (collided && i < 10);

	return Vector(x, y, 0);
}
