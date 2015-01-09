//
// WulfGame/Game/Entity.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "Game/Entity.h"
#include "Game/Physics/Manager.h"

using namespace Wulf;

Entity::Entity()
	: mPos(0, 0, 0)
	, mDir(1, 0, 0)
	, mBounds(0.75, 0.75, 1)
	, Health(100)
	, cmgr(Physics::Manager::GetInstance())
{
	// honk
}

void Entity::Move(const Vector& vel)
{
	mPos += cmgr.CollisionClamp(*this, vel);
}

void Entity::TakeDamage(byte damage)
{
	if (damage > Health)
		damage = Health;
	Health -= damage;
}

/*
Entity::Entity(Entity&& other)
	: mPos(other.mPos)
	, mDir(other.mDir)
	, mBounds(other.mBounds)
	, mHealth(other.mHealth)
	, cmgr(other.cmgr)
{
	// wipe other
	other.mPos = Vector();
	other.mDir = Vector();
	other.mBounds = Vector();
	other.mHealth = 0;
}
Entity& Entity::operator= (Entity&& other) {
	mPos = other.mPos;
	mDir = other.mDir;
	mBounds = other.mBounds;
	mHealth = other.mHealth;
	// wipe other
	other.mPos = Vector();
	other.mDir = Vector();
	other.mBounds = Vector();
	other.mHealth = 0;
	return *this;
}
*/
