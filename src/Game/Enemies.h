//
// WulfGame/Game/Enemies.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once

#include "Game/Constants.h"
#include "Game/Entity.h"
#include "Game/Player.h"
#include "Map/Map.h"
#include "OpenGL/RenderChunk.h"
#include "OpenGL/ResourceManager.h"

namespace Wulf {
    namespace Enemies {
        enum Sprite {
            Guard,
            Officer,
            SS,
            Dog,
            Mutant
        };
        class Enemy : public Entity {
        public:
            virtual ~Enemy() {}
            virtual void Think(double dtime) = 0;
            
            void SetSprite(const Sprite newspr) { mSprite = newspr; }
            Sprite GetSprite() const { return mSprite; }
            byte GetActivity() const { return mActivity; }
        protected:
            Sprite mSprite;
            byte mActivity;
            virtual void Attack() = 0;
        };
        void RegisterEntities();
        OpenGL::RenderChunk* GetRenderChunk(OpenGL::ResourceManager& mgr, glm::mat4 const& projMat);
        std::vector<Enemy*> SpawnRelevent(const Map::Map& map, const Player& ply);
    }
}
