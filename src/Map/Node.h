//
// WulfGame/Map/Node.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once

#include "WulfConstants.h"
#include "Map/Constants.h"
#include "Map/Doors.h"
#include "Map/Wall.h"
#include <utility>
#include <memory>

namespace Wulf {
	namespace Map {
		class Wall;
		class Node {
		public:
			Node(const coord x, const coord y, const word blockdata, const word metadata);
			void AddNeighbour(const Direction direction, Node& neighbour, const bool callback = false);
			Direction GetSpawnDirection() const;

			std::unique_ptr<Wall> GenWall(const Direction dir);

			// Movement
			Node(Node&& other);
			Node& operator=(Node&& other);

			// Wall Generation

			coord x;
			coord y;
			word blockdata;
			word metadata;
			word area;

			bool visibleWall;
			bool wall;
			bool solid;

			bool pickup;

			bool spawn;

			bool door;

			Material material;
			Sprites::SpriteNum sprite;

			bool walls[4];

			Doors::DoorInfo* doorInfo;
			Node *neighbours[4];

		private:

			bool htested;
			bool vtested;

			void ParseMainData();
			void ParseMetaData();

			void vWallStart(Wall*& wall);
			void hWallStart(Wall*& wall);

			void vWallWalk(const Material mat, const bool air, const Node*& last);
			void hWallWalk(const Material mat, const bool air, const Node*& last);
		};
	}
}

