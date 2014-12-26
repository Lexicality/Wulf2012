//
// WulfGame/Game/Entity.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include "WulfConstants.h"
#include "Game/CollisionManager.h"

namespace Wulf {
	class CollisionManager;
	class Entity {
		friend class EntityManager;
	public:
		Entity();
		virtual ~Entity() {}

		Vector GetPos() const { return mPos; }
		void SetPos(const Vector& pos) { mPos = pos; }
		void Move(const Vector& vel);

		Vector GetHeading() const { return mDir; }
		void SetHeading(const Vector& heading) { mDir = heading; }

		bool IsAlive() const { return mHealth > 0; }
		byte GetHealth() const { return mHealth; }
		void SetHealth (const byte health) { mHealth = health; }
		void TakeDamage(const byte damage) { mHealth -= damage; }

		Vector GetCollisionBounds() const { return mBounds; }
		void SetCollisionBounds(const Vector& bounds) { mBounds = bounds; }

		virtual void Think(double dtime) {}

		EntID GetEntID() const { return mEntID; }

		// Allow moving
		// Entity(Entity&& other);
		// Entity& operator= (Entity&& other);

	protected:
		Vector mPos;
		Vector mDir;
		Vector mBounds;
		byte mHealth;
		const CollisionManager& cmgr;

	private:
		// No copying
		Entity(const Entity& other);
		Entity& operator=(const Entity& other);

		EntID mEntID;
	};
}

