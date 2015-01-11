//
// WulfGame/Game/Physics/Manager.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include <vector>
#include <unordered_map>
#include <utility>
#include <set>

#include "WulfConstants.h"
#include "Map/Map.h"
#include "Map/Node.h"
#include "Game/Entity.h"
#include "Game/Player.h"
#include "Game/Pickups.h"
#include "OpenGL/HasRenderChunk.h"
#include "Game/Physics/Debugger.h"

namespace Wulf {
	namespace Physics {
		class TileData;
		class Manager {
		public:
			static Manager& GetInstance() { return instance; };

			void SetMap(const Map::Map& map);

			Vector CollisionClamp(const Entity& ent, const Vector& velocity) const;

			void UpdateDoor(const coords pos, const bool state);

			PickupData* CheckPickup(Player const& ply) const;

			// Adds a pickup to the nearest valid tile to pos
			void AddPickup(coords const pos, PickupData* pickup);
			void AddPickup(Vector const pos, PickupData* pickup);
			// Removes a pickup that the player has picked up
			void RemovePickup(coords const pos);

			OpenGL::RenderChunk * GetRenderChunk(OpenGL::ResourceManager& mgr, glm::mat4 const& proj) { return debugger.GetRenderChunk(mgr, proj); }

			~Manager();

		private:
			// Instancing
			static Manager instance;
			Manager() {};
			Manager(const Manager& other);
			Manager& operator=(const Manager& other);

			std::unordered_map<coords, TileData*> map;
			std::unordered_map<coords, PickupData*> pickups;

			// Get the 9 cloest tiles to this one (including this one)
			typedef std::set<TileData const*, std::function<bool(TileData const*, TileData const*)>> TileSet;
			TileSet grabTiles(const Vector& pos) const;

			Debugger debugger;
		};
	}
}
