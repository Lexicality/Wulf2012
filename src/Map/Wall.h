//
// WulfGame/Map/Wall.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once

#include <vector>

#include "WulfConstants.h"
#include "Map/Node.h"

namespace Wulf {
	namespace Map {
		class Node;
		class Wall {
		public:
			Wall(const Direction direction);
			void SetStart(const Node& start);
			void SetEnd  (const Node& end  );
			void SetMaterial(const Material material);

			// Enable movement
			Wall(Wall&& other);
			Wall& operator=(Wall&& other);

			Material material;
			Direction direction;
			const Node *start;
			const Node *end;
			std::vector<Vector> points;
			std::vector<Vector> stps;

		private:
			void generateSTPs();
		};
	}
}
