#include "Game/Physics/TileData.h"

using namespace Wulf;
using namespace Wulf::Physics;

TileData::TileData(Map::Node const& node)
	: Solid(node.solid), x(node.x), y(node.y)
	, center(node.x, node.y, 0), Bounds({ x - 0.5f, y - 0.5f, x + 0.5f, y + 0.5f })
	, Type(NodeToTile(node))
{
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
