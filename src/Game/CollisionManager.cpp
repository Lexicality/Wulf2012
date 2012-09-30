//
// WulfGame/Game/CollisionManager.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "CollisionManager.h"

#include <stdexcept>
#include <algorithm>
#include <cmath>

#ifdef DEBUG
#include <iostream>
#endif

using namespace Wulf;
using std::for_each;

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
struct CollisionManager::TileData {
	bool Solid;
	bool Sprite; // Solid sprites are nonsolid for traces.
	coord x;
	coord y;
	union {
		CollisionObj Bounds;
		TraceObj Rectangle;
	};
};
CollisionManager CollisionManager::instance;

CollisionManager::~CollisionManager()
{
	for_each(map.begin(), map.end(), [](const MapType::value_type& value) {
		delete value.second;
	});
	delete[] indicies;
	delete[] counts;
}
void CollisionManager::SetMap(const Map::Map& map)
{
	//const std::vector<std::vector<MapNode> >& nodes = map.nodes;
	const auto& nodes = map.nodes;
	if (this->map.max_size() < nodes.size() * nodes[0].size()) {
		throw std::runtime_error("Collision map cannot hold nodemap!");
	}
	this->map.clear();
	for (auto xitr = nodes.begin(), xend = nodes.end(); xitr != xend; ++xitr) {
		for_each(xitr->begin(), xitr->end(), [this](const Map::Node& node) {
			this->map.insert(prep(node));
		});
	}
}

CollisionObj getObj(const Entity& ent)
{
	Vector pos = ent.GetPos();
	float x = pos.x;
	float y = pos.y;
	Vector bounds = ent.GetCollisionBounds();
	float w = bounds.x / 2;
	float h = bounds.y / 2;
	CollisionObj res = {
		// left
		x - w,
		// top
		y - h,
		// right
		x + w,
		// bottom
		y + h
	};
	return res;
}

/*
 * TODO:
 * Instead of selecting the 9 ajacent blocks and merging their faces
 *  consider merging all blocks at the start and then selecting the close ones.
 * This probably needs some kind of spatial management.
 */

Vector CollisionManager::CollisionClamp(const Entity& entity, const Vector& velocity) const
{
    // Uh.
    static const std::function<float(TileData*&,Vector&)> distance = [](TileData*& tile, Vector& pos) -> float
    {
        return (Vector(tile->x, tile->y, 0) - pos).length();
    };
	CollisionObj ent = getObj(entity);

     Vector pos = entity.GetPos();
	const KeyType currentNode = key(entity.GetPos());
	std::vector<MapType::mapped_type> pnodes = grabTiles(currentNode);
    
	// Merge ajacent faces
	for_each(pnodes.begin(), pnodes.end(), [&pnodes](CollisionManager::MapType::mapped_type& tile1) {
		if (tile1 == nullptr)
			return;
		for_each(pnodes.begin(), pnodes.end(), [&tile1](CollisionManager::MapType::mapped_type& tile2) {
			if (tile2 == nullptr || tile1 == tile2)
				return;
			auto& b1 = tile1->Bounds;
			auto& b2 = tile2->Bounds;
			if (b1.Top == b2.Top && b1.Bottom == b2.Bottom) {
				if (b1.Right == b2.Left)
					b1.Right = b2.Right;
				else if (b1.Left == b2.Right)
					b1.Left = b2.Left;
				else
					return;
			} else if (b1.Left == b2.Left && b1.Right == b2.Right) {
				if (b1.Top == b2.Bottom)
					b1.Top = b2.Top;
				else if (b1.Bottom == b2.Top)
					b1.Bottom = b2.Bottom;
				else
					return;
			} else {
				return;
			}
			tile2 = nullptr;
		});
	});
    
    // Sort the tiles so the player collides with them in order
    std::sort(pnodes.begin(), pnodes.end(), [&pos](MapType::mapped_type& a, MapType::mapped_type& b) -> bool
    {
       // Push nullptrs to the back of the list
       if (a == nullptr) // Null is heavier than everything
           return false;
       if (b == nullptr) // Everything is lighter than null
           return true;
       return distance(a, pos) < distance(b, pos);
    });

    // dump all the null pointers
	std::vector<MapType::mapped_type> nodes;
	nodes.reserve(pnodes.size());
	for_each(pnodes.begin(), pnodes.end(), [&nodes](MapType::mapped_type& tile) {
		if (tile != nullptr)
			nodes.push_back(tile);
	});

	// Debuggering
	//UpdateScreen(entity, nodes);

	bool collided = false;
	const TileData *ctile;

	float x = velocity.x;
	float y = velocity.y;
	do {
		collided = false;
		auto itr = nodes.begin(), end = nodes.end();
		for (; itr != end; ++itr) {
			ctile = *itr;
            /*
			if (ctile == nullptr)
				continue;

			if (!ctile->Solid)
				continue;
            */
			const CollisionObj& tile = ctile->Bounds;
			// Basic checks
			// Note, this is in GUI space not cartesean
			if (ent.Left + x >= tile.Right)
				continue;
			if (ent.Right + x <= tile.Left)
				continue;
			// positive Y is DOWN.
			if (ent.Bottom + y <= tile.Top)
				continue;
			if (ent.Top + y >= tile.Bottom)
				continue;

			collided = true;

			float xchange = 0;
			float ychange = 0;
			if (x > 0)
				// Collision from the right to the left ( > )
				xchange = tile.Left - (ent.Right + x);
			else
				// Collision from the left to the right ( < )
				xchange = (tile.Right - ent.Left) - x;

			if (y > 0)
				// Collision from above to below ( v )
				ychange = tile.Top - (ent.Bottom + y);
			else
				// Collision from below to above ( ^ )
				ychange = (tile.Bottom - ent.Top) - y;

			// Only punt the smallest distance possible
			if (fabs(ychange) < fabs(xchange))
				y += ychange;
			else
				x += xchange;

			// Reboot the loop with the new velocity
			break;
		}
	} while (collided);
	
	return Vector(x, y, 0);
}

CollisionObj adjust(const CollisionObj& original, Vector centre)
        {
	float x = centre.x;
	float y = centre.y;
	CollisionObj ret = {
		original.Left - x,
		original.Top - y,
		original.Right - x,
		original.Bottom - y
	};
	return ret;
}

void add(std::vector<float>& buff, const CollisionObj& obj)
        {
	buff.push_back(obj.Left);
	buff.push_back(obj.Bottom);
	buff.push_back(obj.Left);
	buff.push_back(obj.Top);
	buff.push_back(obj.Right);
	buff.push_back(obj.Top);
	buff.push_back(obj.Right);
	buff.push_back(obj.Bottom);
}

// 2 = number of floats/point; 4 = number of points/square
const size_t pointsPerSquare = 2 * 4;
const size_t maxNumSquares = 10;

/*
void CollisionManager::UpdateScreen(const Entity& ply, const std::vector<TileData*>& tiles) const
 {
	std::vector<float> buffdata;
	buffdata.reserve(pointsPerSquare * maxNumSquares);
	const Vector centre = ply.GetPos();
	// Woo!~
	add(buffdata, adjust(getObj(ply), centre));
	for_each(tiles.begin(), tiles.end(), [&buffdata, &centre](const TileData *const tile) {
		//if (tile != nullptr && tile->Solid)
			add(buffdata, adjust(tile->Bounds, centre));
	});
	// Adjust the draw calls etc.
	numSquares = buffdata.size() / pointsPerSquare;
	// WOO random opengl
	GLint prevBuff;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prevBuff);
	glBindBuffer(GL_ARRAY_BUFFER, mRenderChunk.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * buffdata.size(), &buffdata[0]);
	glBindBuffer(GL_ARRAY_BUFFER, prevBuff);
}
*/

OpenGL::RenderChunk * CollisionManager::GetRenderChunk(OpenGL::ResourceManager& mgr, glm::mat4 const&)
{
	// Setup
	mRenderChunk.VAO = mgr.CreateVAO();
	mRenderChunk.VBO = mgr.CreateVBO();
	mRenderChunk.Program = mgr.LoadShaders("CollisionDebug", "SolidColour");
	// We don't use a view uniform
	mRenderChunk.ViewUniform = -1;

	// Programatical stuffs
	glUseProgram(mRenderChunk.Program);
	{
		GLuint colourloc = glGetUniformLocation(mRenderChunk.Program, "Colour");
		glUniform3i(colourloc, 234, 92, 37);
	}
	glUseProgram(0);

	// OpenGL buffery stuffery
	glBindVertexArray(mRenderChunk.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, mRenderChunk.VBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBufferData(GL_ARRAY_BUFFER, pointsPerSquare * maxNumSquares * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// OpenGL render function
	indicies = new GLint[maxNumSquares];
	counts = new GLsizei[maxNumSquares];

	for (int i = 0; i < maxNumSquares; ++i) {
		indicies[i] = i * 4;
		  counts[i] = 4;
	}
	/*
	mRenderChunk.RenderFunction = [this](const RenderChunk& self) {
		if (numSquares > 0)
			glMultiDrawArrays(GL_LINE_LOOP, indicies, counts, numSquares);
	};
	*/

	return &mRenderChunk;
}

void CollisionManager::UpdateDoor(const coord x, const coord y, const bool state)
{
	map[key(x, y)]->Solid = state;
}

inline 
CollisionManager::KeyType CollisionManager::key(const Map::Node& node) const
{
	return std::make_pair(node.x, node.y);
}

inline
CollisionManager::KeyType CollisionManager::key(const coord x, const coord y) const
{
	return std::make_pair(x, y);
}

inline
CollisionManager::KeyType CollisionManager::key(const float x, const float y) const
{
	return std::make_pair(static_cast<coord>(std::floor(x)), static_cast<coord>(std::floor(y)));
}

inline
CollisionManager::KeyType CollisionManager::key(const Vector& pos) const
{
	return key(pos.x, pos.y);
}

inline
CollisionManager::MapType::value_type CollisionManager::prep(const Map::Node& node) const
{
	coord x_ = node.x, y_ = node.y;
	TileData tempdata = {
		node.solid,
		node.sprite != StaticSprites::NONE,
		x_,
		y_,
		{
			{ 
				x_ - 0.5f,
				y_ - 0.5f,
				x_ + 0.5f,
				y_ + 0.5f
			}
		}
	};
	// woo
	TileData *data = new TileData;
	*data = tempdata;
	return std::make_pair(key(node), data);
}

std::vector<CollisionManager::TileData*> CollisionManager::grabTiles(const CollisionManager::KeyType& pos) const
{
	std::vector<TileData*> ret;
	coord x_ = pos.first, y_ = pos.second;
	const auto& none = map.end();
	for (coord x = x_-1; x <= x_+1; ++x) {
		for (coord y = y_-1; y <= y_+1; ++y) {
			const auto& itr = map.find(key(x, y));
			TileData *res = (itr != none) ? itr->second : nullptr;
            if (res != nullptr && res->Solid)
                ret.push_back(res);
		}
	}
	return ret;
}