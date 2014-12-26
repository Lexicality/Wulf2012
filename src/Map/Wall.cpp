//
// WulfGame/Map/Wall.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "Map/Wall.h"

#undef DEBUG_WALLS_SPAM
#undef DEBUG_HITLER
#ifdef DEBUG_HITLER
#pragma message("Hitler debugging enabled.")
#define HITLER_WALL 7
#endif
#ifdef DEBUG_WALLS_SPAM
	#pragma message("Wall spamming enabled")
	std::string directionName(const Direction dir);
	std::string vertOrHori(const Direction dir);
#endif

using namespace Wulf;

Map::Wall::Wall(const Direction direction)
	: material(0), direction(direction)
	, start(nullptr), end(nullptr)
	, points(4), stps(4)
{
#ifdef DEBUG_WALLS_SPAM
	std::cout << "Defining a " << directionName(direction) << " facing " << vertOrHori(direction) << " wall ";
#endif
}

void Map::Wall::SetStart(const Node& start)
{
	this->start = &start;
#ifdef DEBUG_HITLER
	if (material == HITLER_WALL || material < -1)
		std::cout << "HITLER DEBUG! Orginal start: " << start.x << ',' << start.y << '.' << std::endl;
#endif
	float x = start.x;
	float y = start.y;
	switch (direction) {
	case Direction::South:
	case Direction::North:
		y += 1;
		break;
	case Direction::West:
	case Direction::East:
		x -= 1;
		break;
	}
	// The walls are on the edges of the tiles not the middle.
	x += 0.5;
	y -= 0.5;
	switch (direction) {
	case Direction::North:
	case Direction::East:
		points[1] = Vector(x, y, 0);
		points[2] = Vector(x, y, 1);
		break;
	case Direction::South:
	case Direction::West:
		points[0] = Vector(x, y, 0);
		points[3] = Vector(x, y, 1);
	}
#ifdef DEBUG_WALLS_SPAM
	std::cout << "\tStart: " << -x << "," << y << std::endl;
#endif
	generateSTPs();
}

void Map::Wall::SetEnd(const Node& end)
{
	this->end = &end;
#ifdef DEBUG_HITLER
	if (material == HITLER_WALL || material < -1)
		std::cout << "HITLER DEBUG! Orginal end: " << end.x << ',' << end.y << '.' << std::endl;
#endif
	float x = end.x;
	float y = end.y;
	// The walls are on the edges of the tiles not the middle.
	x -= 0.5;
	y += 0.5;
	switch (direction) {
	case Direction::North:
	case Direction::East:
		points[0] = Vector(x, y, 0);
		points[3] = Vector(x, y, 1);
		break;
	case Direction::South:
	case Direction::West:
		points[1] = Vector(x, y, 0);
		points[2] = Vector(x, y, 1);
	}
#ifdef DEBUG_WALLS_SPAM
	std::cout << "\tEnd: " << -x << "," << y << std::endl;
#endif
	generateSTPs();
}

void Map::Wall::SetMaterial(const Material material)
{
	this->material = material;//(material - 1) * 2;
	if (direction == Direction::North || direction == Direction::South) {
		if (material != 40)
			this->material++;
	} else if (material == 40)
		this->material++;

#ifdef DEBUG_WALLS_SPAM
	std::cout << "of material " << material << std::endl;
#endif
}

Map::Wall::Wall(Map::Wall&& other)
	: material(other.material), direction(other.direction)
	, start(other.start), end(other.end)
	, points(std::move(other.points)), stps(std::move(other.stps))
{
}

Map::Wall& Map::Wall::operator=(Map::Wall&& other)
{
	material  = other.material;
	direction = other.direction;
	start     = other.start;
	end       = other.end;
	std::swap(points, other.points);
	std::swap(stps,   other.stps);
	return *this;
}

void Map::Wall::generateSTPs()
{
	if (start == nullptr || end == nullptr)
		return;
	Material mat = material;
	unsigned short int len;
	switch (direction) {
	case Direction::South:
	case Direction::North:
		len =  start->x - end->x + 1;
		break;
	case Direction::West:
	case Direction::East:
		len = end->y - start->y + 1;
	}
	stps[0] = Vector(0,   1, mat);
	stps[1] = Vector(len, 1, mat);
	stps[2] = Vector(len, 0, mat);
	stps[3] = Vector(0,   0, mat);
}

#ifdef DEBUG_WALLS_SPAM
std::string directionName(const Direction dir)
{
	switch (dir) {
	case Direction::South:
		return "south";
	case Direction::West:
		return "west";
	case Direction::East:
		return "east";
	case Direction::North:
		return "north";
	}
	return "invalid";
}
std::string vertOrHori(const Direction dir)
{
	if (dir == Direction::South || dir == Direction::North)
		return "horisontal";
	return "vertical";
}
#endif
