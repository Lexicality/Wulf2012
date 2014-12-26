//
// WulfGame/Game/EntityManager.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "Game/EntityManager.h"

#include <sstream>

using namespace Wulf;

EntityManager EntityManager::instance;

EntityManager::~EntityManager()
{
	PurgeEntities();
	// purge factories
	auto itr = factories.begin(), end = factories.end();
	for (; itr != end; ++itr) {
		delete itr->second;
	}
}

void EntityManager::PurgeEntities()
{
	auto itr = ents.begin(), end = ents.end();
	for (; itr != end; ++itr) {
		delete *itr;
	}
}

void EntityManager::Think(double dtime)
{
	auto itr = ents.begin(), end = ents.end();
	for (; itr != end; ++itr) {
		(*itr)->Think(dtime);
	}
}

Entity* EntityManager::CreateEntity(const std::string& classname, Vector pos, Vector heading)
{
	auto itr = factories.find(classname);
	if (itr == factories.end())
		throw InvalidClassname("Unable to locate factory '" + classname + "'!");
	Entity *ent = itr->second->Create();
	ents.push_back(ent);
	// !!!!Never under any circumstances remove anything from ents unless you remove everything!!!!
	ent->mEntID = ents.size();
	ent->SetPos(pos);
	ent->SetHeading(heading);
	return ent;
}

Entity* EntityManager::GetEntity(EntID entnum) const
{
    try {
        return ents.at(entnum);
    } catch (std::out_of_range&) {
        std::ostringstream s;
        s << "'" << entnum << "' is not a valid ent handle!";
		throw InvalidEntID(s.str());
    }
}
