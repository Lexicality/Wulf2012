#include "Game/Pickups.h"

using namespace Wulf;

std::map<Pickup, PickupData> pickups;

PickupData::PickupData(StaticSprites::StaticSprite s)
	: Sprite(s)
	, Health(0), Ammo(0), Score(0), Weapon(Weapon::None)
{
}

Pickup Wulf::SpriteToPickup(StaticSprites::StaticSprite s)
{
	switch (s) {
	case StaticSprites::FOOD_TURKEY:
		return Pickup::Food;
	case StaticSprites::GUN_AMMO:
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
		PickupData p(StaticSprites::FOOD_TURKEY);
		p.Health = 10;
		pickups.emplace(
			Pickup::Food,
			p
		);
	}
	{
		PickupData p(StaticSprites::GUN_AMMO);
		p.Ammo = 10;
		pickups.emplace(
			Pickup::Ammo,
			p
		);
	}
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