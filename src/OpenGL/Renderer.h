//
// WulfGame/OpenGL/Renderer.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once

// STL
#include <vector>

// OpenGL
#include "gl.h"
#include <glm/glm.hpp>

// Wulf
#include "WulfConstants.h"
#include "Map/Map.h"
#include "OpenGL/ResourceManager.h"
#include "OpenGL/RenderChunk.h"
#include "OpenGL/HasRenderChunk.h"
#include "OpenGL/FontRenderer.h"
#include "Game/Player.h"
#include "Game/Enemies.h"

// Boosts
#include <boost/ref.hpp>

namespace Wulf {
    namespace OpenGL {
        class Renderer {
            struct WallsRenderChunk : public RenderChunk {
                void mRenderFunction(const OpenGL::RenderChunk&) const;
                std::vector<GLint>   *vFirsts;
                std::vector<GLsizei> *vCounts;
            };
        public:
             Renderer();
            ~Renderer();
            double Render();
            void UpdatePlayerInfo(const Player& ply);

            bool IsWindowStillOpen() const;

            void AddRenderChunk(RenderChunk *chunk);
            void AddRenderChunk(HasRenderChunk& chunkable);
            void AddRenderChunk(std::function<RenderChunk*(ResourceManager&, glm::mat4 const&)> chunkfunc);

            void SetMap(const Map::Map& map);
            void UpdateDoor(word doornum, byte openPercent);
            // void AddSprite(Sprite sprite);

        private:

            void LoadShaders();

            // Window related things
            unsigned short int windowWidth;
            unsigned short int windowHeight;

            // View related things
            glm::mat4 getViewMatrix() const;
            glm::mat4 projectionMatrix;
            glm::mat4 viewMatrix;
            bool      bLostViewFocus;

            glm::vec3 vPlyPos;
            glm::vec3 vPlyDir;
#ifdef FREE_VIEW
            glm::vec3 vPlyUp;
#endif

            std::vector<RenderChunk *> chunks;

            RenderChunk Floor, Statics, Doors, Enemies;
            WallsRenderChunk Walls;


            // Timing
            double ltime;
            
            // Rendery stuff
            bool                 bRendering; 
            std::vector<GLint>   vFirsts;
            std::vector<GLsizei> vCounts;

            // Misc
            glm::mat4 mIdentity;
            ResourceManager mgr;
            FontRenderer    fnt;
            char *strbuff;



        };
    }
}

