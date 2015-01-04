#include "Game/Physics/TileData.h"

using namespace Wulf;
using namespace Wulf::Physics;

TileData::TileData(coords const topLeft, coords const bottomRight, TileType type)
	: Solid(type == TileType::Wall || type == TileType::Sprite), Type(type)
	, center((topLeft.x + bottomRight.x) / 2.0, (topLeft.y + bottomRight.y) / 2.0, 0)
	, Bounds({ topLeft.x - 0.5f, topLeft.y - 0.5f, bottomRight.x + 0.5f, bottomRight.y + 0.5f })
{
	// FIXME: TODO
}

TileType Physics::NodeToTile(Map::Node const& node)
{
	if (node.door) {
		return TileType::Door;
	} else if (node.wall) {
		return TileType::Wall;
	} else if (node.solid) {
		return TileType::Sprite;
	} else if (node.pickup) {
		return TileType::Pickup;
	}
	return TileType::Empty;
}
