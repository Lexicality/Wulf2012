#include "Game/Pickups.h"

using namespace Wulf;
namespace Sprites = Wulf::Map::Sprites;

std::map<Pickup, PickupData> pickups;

PickupData::PickupData(Pickup type, Sprites::SpriteNum s)
	: Sprite(s), Type(type)
	, Health(0), Ammo(0), Score(0), Weapon(Weapon::None)
{
}

Pickup Wulf::SpriteToPickup(Sprites::SpriteNum s)
{
	switch (s) {
	case Sprites::FOOD_TURKEY:
		return Pickup::Food;
	case Sprites::GUN_AMMO:
		return Pickup::Ammo;
	// TODO - Everything else
	default:
		return Pickup::None;
	}
}

void setupSprites()
{
	// Serious hate.
	{
		PickupData p(Pickup::Food, Sprites::FOOD_TURKEY);
		p.Health = 10;
		pickups.emplace(
			p.Type,
			p
		);
	}
	{
		PickupData p(Pickup::Ammo, Sprites::GUN_AMMO);
		p.Ammo = 8;
		pickups.emplace(
			p.Type,
			p
		);
	}
	{
		PickupData p(Pickup::Smg, Sprites::GUN_SMG);
		p.Weapon = Weapon::Smg;
		pickups.emplace(
			p.Type,
			p
		);
	}
	{
		PickupData p(Pickup::Chest, Sprites::TREASURE_CHEST);
		p.Score = 1000;
		pickups.emplace(
			p.Type,
			p
		);
	}
	{
		PickupData p(Pickup::Chalice, Sprites::TREASURE_CHALICE);
		p.Score = 100;
		pickups.emplace(
			p.Type,
			p
		);
	}
	// gruel is 4hp
	// blood is 1hp if < 11hp
	// medkit is 25 hp
}

PickupData* Wulf::GetPickup(Pickup pickup)
{
	static bool setup = false;
	if (!setup) {
		setup = true;
		setupSprites();
	}
	auto& ret = pickups.find(pickup);
	if (ret != pickups.end())
		return &ret->second;
	return nullptr;
}
