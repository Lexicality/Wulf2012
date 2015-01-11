//
// WulfGame/Map/Node.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "Map/Node.h"
#include "Map/Doors.h"
#include <cassert>

#define self (*this)

using namespace Wulf;
using Map::Node;
using Map::Wall;
namespace Sprites = Wulf::Map::Sprites;

static Direction reverseDirection(const Direction dir);
static Direction reverseDirection(const int dir);
static bool isSolid(const Sprites::SpriteNum spr);
static bool isPickup(const Sprites::SpriteNum spr);

Node::Node(const coord x, const coord y, const word blockdata, const word metadata)
	: x(-x), y(y), blockdata(blockdata), metadata(metadata), area(0)
	, wall(false), solid(false), pickup(false), spawn(false), door(false), visibleWall(false)
	, material(0), sprite(Sprites::NONE)
	, htested(false), vtested(false)
{
	for (int i = 0; i < 4; ++i) {
		self.walls[i] = false;
		self.neighbours[i] = nullptr;
	}

	ParseMainData();
	ParseMetaData();
}
void Node::ParseMainData()
{
	if (blockdata >= Wulf::FirstWall && blockdata <= Wulf::LastWall) {
		if (Wulf::Doors::IsDoor(blockdata)) {
			door = true;
			// Door like things done by map.cpp
		} else {
			wall = true;
			solid = true;
			// if (blockdata == 0x15) { exit tile }
			material = (blockdata - 1) * 2;
		}
	} else if (blockdata == Wulf::AmbushTile) {
		// TODO: Wtf is this shit
	} else if (blockdata >= Wulf::FirstArea && blockdata <= Wulf::LastArea) {
		// TODO: Areas?
		area = blockdata - Wulf::FirstArea;
	}
}

void Node::ParseMetaData()
{
	if (metadata >= Sprites::First && metadata <= Sprites::Last) {
		// Decorative sprites
		sprite = Sprites::ToSprite(metadata);
		if (isSolid(sprite))
			solid = true;
		if (isPickup(sprite))
			pickup = true;
	} else if (metadata >= 0x13 && metadata <= 0x16) {
		spawn = true;
	} else if (metadata >= 0x5A && metadata <= 0x61) {
		// TODO: Navigation waypoints!
	} else if (metadata == 0x62) {
		// TODO: Secret wall door!
		// For now, just act like ur air
		solid = false;
		wall = false;
		material = 0;
		self.blockdata = 0;
	} else if (metadata == 0x63) {
		// TODO: Exit tile!
	} else if (metadata == 124) {
		// replicate dead-guard-in-wall glitch
		solid = false;
		// TODO: Dead guard
	}
}

std::unique_ptr<Wall> Node::GenWall(const Direction dir)
{
	assert(dir == Direction::East || dir == Direction::South);
	Wall *wall = nullptr;
	if (dir == Direction::East) {
		hWallStart(wall);
	} else if (dir == Direction::South) {
		vWallStart(wall);
	}
	return std::unique_ptr<Wall>(wall);
}

void Node::vWallStart(Wulf::Map::Wall*& wall)
{
	if (vtested || !walls[Direction::East])
		return;

	const bool air = !this->wall;

	wall = new Wall(air ? Direction::West : Direction::East);

	Material mat = material;
	if (air)
		mat = neighbours[Direction::East]->material;

	if (door || (!air && neighbours[Direction::East]->door))
		mat = Wulf::Doors::WallTexture;

	wall->SetMaterial(mat);

	wall->SetStart(*this);

	const Node *last = this;
	// Doors are always 1 unit
	if (mat != Wulf::Doors::WallTexture) {
		vWallWalk(mat, air, last);
	}
	wall->SetEnd(*last);
}

void Node::vWallWalk(const Material mat, const bool air, const Node*& last)
{
	if (!walls[Direction::East])
		return;

	const Node *n = air ? neighbours[Direction::East] : this;
	if (!n->wall || n->material != mat)
		return;

	vtested = true;
	last = this;

	Node *next = neighbours[Direction::South];
	if (next != nullptr)
		next->vWallWalk(mat, air, last);
}

void Node::hWallStart(Wulf::Map::Wall*& wall)
{
	if (htested || !walls[Direction::South])
		return;

	const bool air = !this->wall;

	wall = new Wall(air ? Direction::North : Direction::South);

	Material mat = material;
	if (air)
		mat = neighbours[Direction::South]->material;

	if (door || (!air && neighbours[Direction::South]->door))
		mat = Wulf::Doors::WallTexture;

	wall->SetMaterial(mat);

	wall->SetStart(*this);

	const Node *last = this;
	// Doors are always 1 unit
	if (mat != Wulf::Doors::WallTexture) {
		hWallWalk(mat, air, last);
	}
	wall->SetEnd(*last);
}

void Node::hWallWalk(const Material mat, const bool air, const Node*& last)
{
	if (!walls[Direction::South])
		return;

	const Node *n = air ? neighbours[Direction::South] : this;
	if (!n->wall || n->material != mat)
		return;

	htested = true;
	last = this;

	Node *next = neighbours[Direction::East];
	if (next != nullptr)
		next->hWallWalk(mat, air, last);
}

Direction Node::GetSpawnDirection() const
{
	switch (metadata) {
	case 0x13:
		return Direction::North;
	case 0x14:
		return Direction::East;
	case 0x15:
		return Direction::South;
	case 0x16:
		return Direction::West;
	default:
		return Direction::North;
	}
}

void Node::AddNeighbour(const Direction direction, Node& neighbour, const bool callback)
{
	// Remember them
	self.neighbours[direction] = &neighbour;
	// Check for walls
	if ((self.wall != neighbour.wall) && (self.blockdata != neighbour.blockdata)) {
		self.walls[direction] = true;
		self.visibleWall = true;
	}
	// Poke them
	if (!callback)
		neighbour.AddNeighbour(reverseDirection(direction), self, true);
}

Node::Node(Node&& other)
	: x(other.x), y(other.y)
	, blockdata(other.blockdata), metadata(other.metadata), area(other.area)
	, wall(other.wall), solid(other.solid), visibleWall(other.visibleWall)
	, pickup(other.pickup), spawn(other.spawn), door(other.door)
	, material(other.material), sprite(other.sprite)
	, htested(other.htested), vtested(other.vtested)
{
	for (int i = 0; i < 4; ++i) {
		self.walls[i] = other.walls[i];
		self.neighbours[i] = other.neighbours[i];
		if (self.neighbours[i] != nullptr)
			self.neighbours[i]->neighbours[reverseDirection(i)] = this;
	}
}

Node& Node::operator=(Node&& other)
{
	if (other.x == self.x && other.y == self.y)
		return self;
	self.x = other.x;
	self.y = other.y;

	self.blockdata = other.blockdata;
	self.metadata = other.metadata;

	self.material = other.material;
	self.sprite = other.sprite;

	self.visibleWall = other.visibleWall;
	self.wall = other.wall;
	self.solid = other.solid;
	self.door = other.door;
	self.spawn = other.spawn;
	self.pickup = other.pickup;

	self.area = other.area;

	self.htested = other.htested;
	self.vtested = other.vtested;

	for (int i = 0; i < 4; ++i) {
		self.walls[i] = other.walls[i];
		self.neighbours[i] = other.neighbours[i];
		if (self.neighbours[i] != nullptr)
			self.neighbours[i]->neighbours[reverseDirection(i)] = this;
	}
	return self;
}

static
Direction reverseDirection(const Direction dir)
{
	switch (dir) {
	case Direction::South:
		return Direction::North;
	case Direction::West:
		return Direction::East;
	case Direction::East:
		return Direction::West;
	case Direction::North:
		return Direction::South;
	}
	return Direction::South;
}

// hurr
inline static
Direction reverseDirection(const int dir)
{
	return reverseDirection(static_cast<Direction>(dir));
}

static
bool isSolid(const Sprites::SpriteNum spr)
{
	switch (spr) {
	case Sprites::BARREL_GREEN:
	case Sprites::TABLE_CHAIRS:
	case Sprites::LAMP_FLOOR:
	case Sprites::SKELETON_HUNG:
	case Sprites::PILLAR:
	case Sprites::POT_TREE:
	case Sprites::SINK:
	case Sprites::POT_PLANT:
	case Sprites::POT_URN:
	case Sprites::TABLE_BARE:
	case Sprites::ARMOR:
	case Sprites::CAGE_EMPTY:
	case Sprites::CAGE_SKELETON:
	case Sprites::BED:
	case Sprites::BARREL_WOOD:
	case Sprites::WELL_FULL:
	case Sprites::WELL_EMPTY:
	case Sprites::FLAG:
	case Sprites::STOVE:
	case Sprites::SPEARS:
		return true;
	default:
		return false;
	}
}

static
bool isPickup(const Sprites::SpriteNum spr)
{
	switch (spr) {
	case Sprites::FOOD_GRUEL:
	case Sprites::KEY_GOLD:
	case Sprites::KEY_BLUE:
	case Sprites::FOOD_TURKEY:
	case Sprites::FIRSTAID:
	case Sprites::GUN_AMMO:
	case Sprites::GUN_SMG:
	case Sprites::GUN_CHAINGUN:
	case Sprites::TREASURE_CHALICE:
	case Sprites::TREASURE_CHEST:
	case Sprites::TREASURE_CROWN:
	case Sprites::NEWLIFE:
	case Sprites::FOOD_GORE_1:
	case Sprites::FOOD_GORE_2:
		return true;
	default:
		return false;
	}
}
