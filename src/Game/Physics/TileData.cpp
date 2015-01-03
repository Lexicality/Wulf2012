#include "Game/Physics/TileData.h"

using namespace Wulf;
using namespace Wulf::Physics;

TileData::TileData(Map::Node const& node)
	: Solid(node.solid), x(node.x), y(node.y)
	, center(node.x, node.y, 0), Bounds({ x - 0.5f, y - 0.5f, x + 0.5f, y + 0.5f })
{
	if (node.door) {
		Type = TileType::Door;
	} else if (node.sprite == StaticSprites::NONE) {
		if (Solid) {
			Type = TileType::Wall;
		} else {
			Type = TileType::Empty;
		}
	} else if (Solid) {
		Type = TileType::SolidSprite;
	} else {
		Type = TileType::Decoration;
	}
}
