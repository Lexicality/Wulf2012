//
// WulfGame/Map/Map.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
// STL
#include <vector>
#include <array>

// OpenGL
#include "gl.h"

// Wulf
#include "WulfConstants.h"
#include "Map/Constants.h"
#include "Map/Node.h"
#include "Map/Wall.h"


namespace Wulf {
	typedef std::vector<Vector> VectorVector;
    namespace Map {
        class Map {
        public:

            // funcs
            Map(const word mapNum);
            GLuint GetPackedQuads  (VectorVector& packed) const;
            GLuint GetPackedSprites(std::vector<short int>& packed) const;
            GLuint GetPackedDoors  (std::vector<short int>& packed) const;
            
            // Vars
            std::array<std::vector<Node>, 64> nodes;
            std::vector<Wall> walls;
            std::vector<StaticSprites::Sprite> sprites;
            std::vector<Doors::DoorInfo> doors;

            Vector floorColour;
            Vector ceilingColour;

            Vector  spawnPos;
            GLfloat spawnAng;

            static const byte width = 64;
            static const byte height = 64;
            static const byte halfwidth  = 32;
            static const byte halfheight = 32;

            std::string parString;
            float       parTime;

            std::string name;
            std::string music;
            
        private:
            std::vector<word> map;
            std::vector<word> objs;
            
            void LoadFile(const word mapNum);
            void ParseNodes();
            void ParseWalls();
            
            void DebugOutput();


        };
    }
}
