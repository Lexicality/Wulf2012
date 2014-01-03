//
// WulfGame/Game/CollisionManager.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include <vector>
#include <map>
#include <utility>

#include <boost/ref.hpp>

#include "WulfConstants.h"
#include "Map/Map.h"
#include "Map/Node.h"
#include "Game/Entity.h"
#include "OpenGL/HasRenderChunk.h"

namespace Wulf {
	class Entity;
	class CollisionManager : public OpenGL::HasRenderChunk {
		struct TileData;
	public:
		static CollisionManager& GetInstance() { return instance; };

		void SetMap(const Map::Map& map);

		Vector CollisionClamp(const Entity& ent, const Vector& velocity) const;

		void UpdateDoor(const coord x, const coord y, const bool state);

		OpenGL::RenderChunk * GetRenderChunk(OpenGL::ResourceManager& mgr, glm::mat4 const&);

		~CollisionManager();

	private:
		// Instancing
		static CollisionManager instance;
		CollisionManager() {};
		CollisionManager(const CollisionManager& other);
		CollisionManager& operator=(const CollisionManager& other);

		// Graphical Overray
		/*
		This is not actually const. It messes with various C apis to render stuff to
		 the screen. However, it needs to be called from a const function so go figure.
		It tries to leave the OGL state as it found it.
		*/
		void UpdateScreen(const Entity& ply, const std::vector<TileData*>& tiles) const;
		mutable GLsizei numSquares;
		GLint *indicies;
		GLsizei *counts;

		// Augauruguughhh
		typedef std::map<std::pair<coord, coord>, TileData*> MapType;
		typedef MapType::key_type KeyType;

		// Various ways of nabbing a key
		KeyType key(const Map::Node& node) const;
		KeyType key(const coord x, const coord y) const;
		KeyType key(const float x, const float y) const;
		KeyType key(const Vector& pos) const;

		// Convert a mapnode to a collision tile
		MapType::value_type prep(const Map::Node& node) const;

		// Get the 9 closest tiles to this one (including this one)
		std::vector<TileData*> grabTiles(const KeyType&) const;
		MapType map;
	};
}
