//
// WulfGame/Map/Node.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "Map/Node.h"
#include <cassert>

#define self (*this)

using namespace Wulf;
using Map::Node;
using Map::Wall;

static Direction reverseDirection(const Direction dir);
static Direction reverseDirection(const int dir);
static bool isSolid(const StaticSprites::StaticSprite spr);
static bool isPickup(const StaticSprites::StaticSprite spr);

Node::Node(const coord x, const coord y, const word blockdata, const word metadata)
: x(-x), y(y), blockdata(blockdata), metadata(metadata), area(0)
, wall(false), solid(false), pickup(false), spawn(false), door(false)
, material(0), sprite(StaticSprites::NONE)
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
    if (metadata >= StaticSprites::First && metadata <= StaticSprites::Last) {
		// Decorative sprites
		sprite = StaticSprites::ToSprite(metadata);
		if (isSolid(sprite))
			solid = true;
        if (isPickup(sprite))
            pickup = true;
	} else if (metadata >= 0x13 && metadata <= 0x16) {
		spawn = true;
	} else if (metadata >= 0x5A && metadata <= 0x61) {
		// TODO: Navigation waypoints!
	}  else if (metadata == 0x62) {
		// TODO: Secret wall door!
		// For now, just act like ur air
		solid = false;
		wall  = false;
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
    assert(dir == DIRECTION_EAST || dir == DIRECTION_SOUTH);
    Wall *wall = nullptr;
    if (dir == DIRECTION_EAST) {
        hWallStart(wall);
    } else if (dir == DIRECTION_SOUTH) {
        vWallStart(wall);
    }
    return std::unique_ptr<Wall>(wall);
}

void Node::vWallStart(Wulf::Map::Wall*& wall)
{
    if (vtested || !walls[DIRECTION_EAST])
        return;
    
    const bool air = !this->wall;
    
    wall = new Wall(air ? DIRECTION_WEST : DIRECTION_EAST);
    
    Material mat = material;
    if (air)
        mat = neighbours[DIRECTION_EAST]->material;
    
    if (door || (!air && neighbours[DIRECTION_EAST]->door))
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
    if (!walls[DIRECTION_EAST])
        return;
    
    const Node *n = air ? neighbours[DIRECTION_EAST] : this;
    if (!n->wall || n->material != mat)
        return;
    
    vtested = true;
    last = this;
    
    Node *next = neighbours[DIRECTION_SOUTH];
    if (next != nullptr)
        next->vWallWalk(mat, air, last);
}

void Node::hWallStart(Wulf::Map::Wall*& wall)
{
    if (htested || !walls[DIRECTION_SOUTH])
    return;
    
    const bool air = !this->wall;
    
    wall = new Wall(air ? DIRECTION_NORTH : DIRECTION_SOUTH);
    
    Material mat = material;
    if (air)
        mat = neighbours[DIRECTION_SOUTH]->material;
    
    if (door || (!air && neighbours[DIRECTION_SOUTH]->door))
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
    if (!walls[DIRECTION_SOUTH])
        return;
    
    const Node *n = air ? neighbours[DIRECTION_SOUTH] : this;
    if (!n->wall || n->material != mat)
        return;
    
    htested = true;
    last = this;
    
    Node *next = neighbours[DIRECTION_EAST];
    if (next != nullptr)
        next->hWallWalk(mat, air, last);
}


Direction Node::GetSpawnDirection() const
{
	switch (metadata) {
		case 0x13:
			return DIRECTION_NORTH;
		case 0x14:
			return DIRECTION_EAST;
		case 0x15:
			return DIRECTION_SOUTH;
		case 0x16:
			return DIRECTION_WEST;
		default:
			return DIRECTION_NORTH;
	}
}

void Node::AddNeighbour(const Direction direction, Node& neighbour, const bool callback)
{
	// Remember them
	self.neighbours[direction] = &neighbour;
	// Check for walls
	if ((self.wall != neighbour.wall) && (self.blockdata != neighbour.blockdata))
		self.walls[direction] = true;
	// Poke them
	if (!callback)
		neighbour.AddNeighbour(reverseDirection(direction), self, true);
}
		
Node::Node(Node&& other)
: x(other.x), y(other.y)
, blockdata(other.blockdata), metadata(other.metadata), area(other.area)
, wall(other.wall), solid(other.solid)
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
	self.x         = other.x;
	self.y         = other.y;

	self.blockdata = other.blockdata;
	self.metadata  = other.metadata;

	self.material  = other.material;
	self.sprite    = other.sprite;

	self.wall      = other.wall;
	self.solid     = other.solid;
	self.door      = other.door;
	self.spawn     = other.spawn;
    self.pickup    = other.pickup;

	self.area      = other.area;

	self.htested   = other.htested;
	self.vtested   = other.vtested;

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
	case DIRECTION_SOUTH:
		return DIRECTION_NORTH;
	case DIRECTION_WEST:
		return DIRECTION_EAST;
	case DIRECTION_EAST:
		return DIRECTION_WEST;
	case DIRECTION_NORTH:
		return DIRECTION_SOUTH;
	}
	return DIRECTION_SOUTH;
}

// hurr
inline static
Direction reverseDirection(const int dir)
{
	return reverseDirection(static_cast<Direction>(dir));
}

static
bool isSolid(const StaticSprites::StaticSprite spr)
{
    switch(spr) {
        case StaticSprites::BARREL_GREEN:
        case StaticSprites::TABLE_CHAIRS:
        case StaticSprites::LAMP_FLOOR:
        case StaticSprites::SKELETON_HUNG:
        case StaticSprites::PILLAR:
        case StaticSprites::POT_TREE:
        case StaticSprites::SINK:
        case StaticSprites::POT_PLANT:
        case StaticSprites::POT_URN:
        case StaticSprites::TABLE_BARE:
        case StaticSprites::ARMOR:
        case StaticSprites::CAGE_EMPTY:
        case StaticSprites::CAGE_SKELETON:
        case StaticSprites::BED:
        case StaticSprites::BARREL_WOOD:
        case StaticSprites::WELL_FULL:
        case StaticSprites::WELL_EMPTY:
        case StaticSprites::FLAG:
        case StaticSprites::STOVE:
        case StaticSprites::SPEARS:
            return true;
        default:
            return false;
    }
}

static
bool isPickup(const StaticSprites::StaticSprite spr)
{
    switch(spr) {
        case StaticSprites::FOOD_GRUEL:
        case StaticSprites::KEY_GOLD:
        case StaticSprites::KEY_BLUE:
        case StaticSprites::FOOD_TURKEY:
        case StaticSprites::FIRSTAID:
        case StaticSprites::GUN_AMMO:
        case StaticSprites::GUN_SMG:
        case StaticSprites::GUN_CHAINGUN:
        case StaticSprites::TREASURE_CHALICE:
        case StaticSprites::TREASURE_CHEST:
        case StaticSprites::TREASURE_CROWN:
        case StaticSprites::NEWLIFE:
        case StaticSprites::FOOD_GORE_1:
        case StaticSprites::FOOD_GORE_2:
            return true;
        default:
            return false;

    }
}
