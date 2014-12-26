//
// WulfGame/Map/Map.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "Map/Map.h"
#include <boost/format.hpp>
#include <boost/iostreams/stream.hpp>
#include <cstring>
#include "Map/CompressionTools.h"
#include "Wrappers/PhysFS.h"

#ifdef DEBUG
//#define DEBUG_MAPSPEW
#endif
#ifdef DEBUG_MAPSPEW
#include <sstream>
#endif

using namespace Wulf;

typedef unsigned int dword;
static word readShort(PhysFS::FileStream& str);
//static dword readLong(// PhysFS::FileStream& str);

Map::Map::Map(const word mapNum)
{
	LoadFile(mapNum);

	ParseNodes();

	DebugOutput();

	ParseWalls();
}

void Map::Map::LoadFile(const word mapNum)
{
	std::string fname = (boost::format("maps/w%02d.map") % mapNum).str();
	if (!PhysFS::exists(fname))
		throw std::runtime_error("No such map " + fname + "!");

	PhysFS::FileStream file(fname, PhysFS::OM_READ);
	// TODO: maybe I should spin my own error messages.
	file.exceptions(PhysFS::FileStream::eofbit | PhysFS::FileStream::failbit | PhysFS::FileStream::badbit);

	char data[6] = {0};
	file.read(data, 4);
	if (std::strcmp(data, "!ID!") != 0)
		throw std::runtime_error(fname + " is not a valid map file!");

	word RLEWTag = readShort(file);
	word width   = readShort(file);
	word height  = readShort(file);
	if (width != 64 && height != 64)
		throw std::runtime_error("Map has invalid dimensions!");

	file.read(data, 4);
	ceilingColour = Vector(data[0], data[1], data[2]);
	ceilingColour /= 0xFF;

	file.read(data, 4);
	floorColour   = Vector(data[0], data[1], data[2]);
	floorColour /= 0xFF;

	word lengths[3];
	file.read(reinterpret_cast<char*>(lengths), 2 * 3);

	dword offsets[3];
	file.read(reinterpret_cast<char*>(offsets), 4 * 3);

	word nameLength = readShort(file);
	word musicLength = readShort(file);

	// if sizeof(float) != 4, shit will arise.
	file.read(reinterpret_cast<char*>(&parTime), 4);

	file.read(data, 5);
	parString = std::string(data);

	std::vector<char> cdata(nameLength + 1, 0);
	file.read(&cdata[0], nameLength);
	name = std::string(&cdata[0]);

	cdata.resize(musicLength + 1, 0);
	file.read(&cdata[0], musicLength);
	music = std::string(&cdata[0]);

	// END OF HEADER

	// Read file
	for (byte i = 0; i < 2; ++i) {
		file.seekg(offsets[i], std::ios_base::beg);
		std::vector<byte> rawdata(lengths[i], 0);
		char *rawptr = reinterpret_cast<char*>(&rawdata[0]);
		file.read(rawptr, lengths[i]);
		std::vector<word> RLEWdata = CompressionTools::CarmackExpand(rawdata);
		std::vector<word> uncompressed = CompressionTools::RLEWExpand(RLEWdata, RLEWTag);
		if (i == 0)
			map = std::move(uncompressed);
		else
			objs = std::move(uncompressed);
	}
}

void Map::Map::ParseNodes()
{
	// Displaced coordinates.
	for (byte x = 0; x < width; ++x) {
		auto& cnodes = nodes[x];
		cnodes.reserve(height);
		for (byte y = 0; y < height; ++y) {
			size_t i = x + y * (word) width;
			cnodes.emplace_back(x - halfwidth, y - halfheight, map[i], objs[i]);
			Node& node = cnodes.back();
			// Space building
			if (x > 0)
				node.AddNeighbour(Direction::West, nodes[x - 1][y]);
			if (y > 0)
				node.AddNeighbour(Direction::North, nodes[x][y - 1]);
			// Spawnpoint
			if (node.spawn) {
				spawnPos = Vector(node.x, node.y, 0.65f);
				switch(node.GetSpawnDirection()) {
				case Direction::North:
					spawnAng = 3.1415f;
					break;
				case Direction::East:
					spawnAng = 3.1415f * 1.5f;
					break;
				case Direction::South:
					spawnAng = 0.0f;
					break;
				case Direction::West:
					spawnAng = 3.1415f * 0.5f;
					break;
				}
			}
			// Static sprites
			if (node.sprite != StaticSprites::NONE) {
				StaticSprites::Sprite spr = {
					node.x,
					node.y,
					node.sprite
				};
				sprites.push_back(spr);
			}
			// Daws
			if (node.door) {
				doors.emplace_back(node.x, node.y, node.blockdata);
				Doors::DoorInfo& doorInfo = doors.back();
				node.doorInfo = &doorInfo;
				doorInfo.doorID = doors.size() - 1;
			}
		}
	}
}

void Map::Map::ParseWalls()
{
	for (auto& xnodes : nodes) {
		for (Node& node : xnodes) {
			// Check for vertical walls
			{
				std::unique_ptr<Wall> ptr = node.GenWall(Direction::South);
				if (ptr != nullptr)
					walls.push_back(std::move(*ptr));
			}
			// Now do the horisontals
			{
				std::unique_ptr<Wall> ptr = node.GenWall(Direction::East);
				if (ptr != nullptr)
					walls.push_back(std::move(*ptr));
			}
		}
	}
}

void Map::Map::DebugOutput()
{
#ifdef DEBUG_MAPSPEW
	std::cout << " vim: nowrap listchars=" << std::endl;
	std::cout.setf(std::ios_base::right);
	std::cout.fill(' ');
	std::cout.width(3);
	std::cout << "  ";
	std::ostringstream bluh;
	bluh << "   ";
	for (byte x = 0; x < width; ++x) {
		std::cout.width(3);
		std::cout << -(x - halfwidth);
		std::cout << ' ';
		bluh << "****";
	}
	std::cout << std::endl;
	std::cout << bluh.str() << std::endl;
	for (byte y = 0; y < height; ++y) {
		std::cout.width(3);
		std::cout << y - halfheight << '*';
		std::ostringstream honk;
		honk << "   *";
		for (byte x = 0; x < width; ++x) {
			Node& node = nodes[x][y];
			const auto& metadata = node.metadata;
			std::cout.width(3);
			// Pickups
			if (node.pickup)
				std::cout << " P ";
			// Sprites
			else if (metadata >= StaticSprites::First && metadata <= StaticSprites::Last)
			{
				if (node.solid)
					std::cout << " S ";
				else
					std::cout << " s ";
			}
			// Spawn point
			else if (metadata >= 0x13 && metadata <= 0x16)
				std::cout << " * ";
			// Waypoints
			else if (metadata == 0x5A)
				std::cout << " > ";
			else if (metadata == 0x5B)
				std::cout << " ^>";
			else if (metadata == 0x5C)
				std::cout << " ^ ";
			else if (metadata == 0x5D)
				std::cout << "<^ ";
			else if (metadata == 0x5E)
				std::cout << " < ";
			else if (metadata == 0x5F)
				std::cout << "<v ";
			else if (metadata == 0x60)
				std::cout << " v ";
			else if (metadata == 0x61)
				std::cout << " v>";
			// Pushwall
			else if (metadata == 0x62)
				std::cout << " D ";
			// Exit
			else if (metadata == 0x63)
				std::cout << " E ";
			// Pre-prepared corpse
			else if (metadata == 124)
				std::cout << " c ";
			else if (node.door)
				std::cout << " # ";
			// Something that won't display on the map
			else if (metadata > 999)
				std::cout << " ! ";
			// Unknown object
			else if (metadata)
				std::cout << metadata;
			else if (node.area)
				std::cout << node.area;
			else if (node.walls[0] || node.walls[1] || node.walls[2] || node.walls[3])
				std::cout << node.material;
			else
				std::cout << ' ';
			std::cout << (node.walls[Direction::East] ? '|' : ' ');
			honk << (node.walls[Direction::South] ? "---" : "   ");
			honk << ' ';
		}
		std::cout << std::endl;
		std::cout << honk.str() << std::endl;
	}
#endif
}

GLuint Map::Map::GetPackedQuads(VectorVector& packed) const
{
	// Make sure we're clean
	packed.clear();
	const GLuint numWalls = walls.size();
	// Each wall has 4 vertexes and 4 stps, making 8 entries
	const GLuint numEntries = numWalls * 8;
	packed.reserve(numEntries);
	for (auto& wall : walls) {
		// Loop through each of the four points and shove them at the back of each one
		for (int i = 0; i < 4; ++i) {
			packed.push_back(wall.points[i]);
			packed.push_back(wall.stps  [i]);
		}
	}
	return numWalls;
}

GLuint Map::Map::GetPackedSprites(std::vector<short int>& packed) const
{
	GLuint res = sprites.size();
	packed.empty();
	packed.reserve(res * 3);

	for (auto& spr : sprites) {
		packed.push_back(spr.x);
		packed.push_back(spr.y);
		packed.push_back(spr.spr);
	}
	return res;
}

GLuint Map::Map::GetPackedDoors(std::vector<short int>& packed) const
{
	// TODO: Implment meo
	GLuint res = doors.size();
	packed.empty();
	packed.reserve(res * 4);
	for (auto& door : doors) {
		packed.push_back(door.x);
		packed.push_back(door.y);
		packed.push_back(static_cast<signed short int>(door.tex));
		if (door.dir == Doors::Vertical)
			packed.push_back(1);
		else
			packed.push_back(0);
	}
	return res;
}

void Map::Map::DoorThink(double dtime) {
	for (auto& door : doors) {
		door.Think(dtime);
	}
}

word readShort(PhysFS::FileStream& str)
{
	word read;
	str.read(reinterpret_cast<char*>(&read), 2);
	return read;
}

/*
dword readLong(PhysFS::FileStream& str)
{
dword read;
str.read(reinterpret_cast<char*>(&read), 4);
return read;
}
*/

