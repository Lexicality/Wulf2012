//
// WulfGame/Game/Game.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include "OpenGL/Renderer.h"
#include "Map/Map.h"
#include "Game/Player.h"
#include "Game/InputManager.h"

namespace Wulf {
    class Game {
    public:
        Game(const Difficulty difficulty);
        ~Game();
        
        void LoadMap(const word MapNum);
        
        bool IsRunning() const;
        
        void Run();
        
    private:
        OpenGL::Renderer rendr;
        Wulf::Input::Manager input;
        Wulf::Player ply;
        Wulf::Map::Map *map;
        bool running;
        double dtime;
    };
}