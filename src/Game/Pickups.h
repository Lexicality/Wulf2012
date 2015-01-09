#pragma once
#include <functional>
#include <map>
#include "WulfConstants.h"
#include "Game/Player.h"
#include "Map/Constants.h"

namespace Wulf {
	struct PickupData {
		StaticSprites::StaticSprite Sprite;
		byte Health;
		byte Ammo;
		byte Score;
		Weapon Weapon;
		PickupData(StaticSprites::StaticSprite s);
	};
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
	std::map<Pickup, PickupData> pickups;
	Pickup SpriteToPickup(StaticSprites::StaticSprite s);
}
