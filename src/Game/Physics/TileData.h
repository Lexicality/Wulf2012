#pragma once
#include "WulfConstants.h"
#include "Map/Node.h"

namespace Wulf {
	namespace Physics {
		enum class TileType {
			Empty = 0,
			Wall,
			Door,
			Sprite,
			Pickup,
		};
		struct CollisionObj {
			float Left;
			float Top;
			float Right;
			float Bottom;
		};
		struct Coordinate {
			float x;
			float y;
		};
		struct TraceObj {
			Coordinate TopLeft;
			Coordinate BottomRight;
		};
		// Represents a rectangular area of the map
		class TileData {
		public:
			bool Solid;
			TileType Type;
			// FIXME: These are meaningless
			coord x, y;
			Vector center;
			union {
				CollisionObj Bounds;
				TraceObj Rectangle;
			};

			// Gets the closest point on the bounds to pos
			Vector ClosestPoint(Vector pos) const;

			TileData(Map::Node const& node);
		};
	}
}
