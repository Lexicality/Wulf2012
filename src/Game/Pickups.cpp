#include "Game/Pickups.h"

using namespace Wulf;

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

int setupSprites()
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
	return 0;
}

// some weird hackage
int a = setupSprites();
