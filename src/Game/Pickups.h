#pragma once
#include <functional>
#include <map>
#include "WulfConstants.h"
#include "Game/Constants.h"
#include "Map/Constants.h"

namespace Wulf {
	enum class Pickup {
		None = 0,
		Ammo,
		OneUp,
		// Healing
		Medkit,
		Food,
		Gruel,
		Blood,
		// Guns
		Smg,
		Minigun,
		// Treasure
		Chalice,
		Chest,
		Crown,
	};
	struct PickupData {
		Pickup Type;
		StaticSprites::StaticSprite Sprite;
		byte Health;
		byte Ammo;
		word Score;
		Weapon Weapon;
		PickupData(Pickup type, StaticSprites::StaticSprite s);
	};
	PickupData* GetPickup(Pickup pickup);
	Pickup SpriteToPickup(StaticSprites::StaticSprite s);
}
