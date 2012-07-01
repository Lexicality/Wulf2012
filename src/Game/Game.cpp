//
// WulfGame/Game/Game.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "Game/Game.h"
#include "Game/CollisionManager.h"

using namespace Wulf;

Game::Game(const Difficulty::Type difficulty)
: running(false), dtime(0)
{
    Wulf::Enemies::RegisterEntities();
    rendr.AddRenderChunk(Wulf::Enemies::GetRenderChunk);
    ply.SetDifficulty(difficulty);
}

Game::~Game()
{
    delete map;
}

void Game::LoadMap(const word mapnum)
{
    delete map;
    map = new Map::Map(mapnum);
    rendr.SetMap(*map);
    ply.ProcessMapInput(*map);
    Wulf::Enemies::SpawnRelevent(*map, ply);
    Wulf::CollisionManager::GetInstance().SetMap(*map);
    running = true;
}

bool Game::IsRunning() const
{
    if (!running)
        return false;
    return rendr.IsWindowStillOpen();
}

void Game::Run()
{
    const Wulf::Input::Data& in = input.GetInput();
    if (in.Exit) {
        running = false;
        return;
    }
    ply.ProcessUserInput(in, dtime);
    rendr.UpdatePlayerInfo(ply);
    dtime = rendr.Render();
}