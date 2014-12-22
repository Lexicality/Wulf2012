//
// WulfGame/Game/Game.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "Game/Game.h"
#include "Game/CollisionManager.h"
#include "Map/Doors.h"

using namespace Wulf;

Game::Game(const Difficulty difficulty)
: map(nullptr), running(false), dtime(0), emgr(EntityManager::GetInstance())
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

	map->doors.at(11).Open();
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
	emgr.Think(dtime);
	map->DoorThink(dtime);
	for (Doors::DoorInfo& door : map->doors) {
		rendr.UpdateDoor(door.doorID, door.openPercent());
	}
    dtime = rendr.Render();
}