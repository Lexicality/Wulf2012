//
// WulfGame/Game/Game.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include <functional>
#include "Game/Game.h"
#include "Map/Doors.h"

using namespace Wulf;

Game::Game(const Difficulty difficulty)
	: map(nullptr), running(false), dtime(0)
	, emgr(EntityManager::GetInstance()), cmgr(Physics::Manager::GetInstance())
{
	Wulf::Enemies::RegisterEntities();
	rendr.AddRenderChunk(Wulf::Enemies::GetRenderChunk);
	//rendr.AddRenderChunk(std::bind(&Wulf::Physics::Manager::GetRenderChunk, &cmgr, std::placeholders::_1, std::placeholders::_2));
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
	cmgr.SetMap(*map);
	running = true;

	//map->doors.at(11).Open();
	for (auto& door : map->doors) {
		door.Open();
	}
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
	PickupData const* pickup = cmgr.CheckPickup(ply);
	if (pickup != nullptr && ply.CanPickup(pickup)) {
		ply.HandlePickup(pickup);
		cmgr.RemovePickup(ply.GetPos());
	}
	if (cmgr.RerenderPickups()) {
		rendr.UpdatePickups(cmgr.GetPickups());
	}
	rendr.UpdatePlayerInfo(ply);
	emgr.Think(dtime);
	map->DoorThink(dtime);
	for (Doors::DoorInfo& door : map->doors) {
		rendr.UpdateDoor(door.doorID, door.openPercent());
		cmgr.UpdateDoor(coords(door.x, door.y), door.isSolid());
	}
	dtime = rendr.Render();
}
