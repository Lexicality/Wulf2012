//
// WulfGame/Map/Constants.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include "WulfConstants.h"

namespace Wulf {
	namespace StaticSprites {
		enum StaticSprite {
			NONE   = -1,
			PUDDLE = 0,
			BARREL_GREEN,
			TABLE_CHAIRS,
			LAMP_FLOOR,
			LAMP_CHANELIER,
			SKELETON_HUNG,
			FOOD_GRUEL,
			PILLAR,
			POT_TREE,
			SKELETON_FLAT,
			SINK,
			POT_PLANT,
			POT_URN,
			TABLE_BARE,
			LAMP_CEILING,
			PANS_1,
			ARMOR,
			CAGE_EMPTY,
			CAGE_SKELETON,
			SKELETON_PILE,
			KEY_GOLD,
			KEY_BLUE, // silver?
			BED,
			POT_TOILET, // literally a pot o' poo
			FOOD_TURKEY,
			FIRSTAID,
			GUN_AMMO,
			GUN_SMG,
			GUN_CHAINGUN,
			TREASURE_CHALICE,
			TREASURE_CHEST,
			TREASURE_CROWN,
			NEWLIFE,
			FOOD_GORE_1, // ewwww
			BARREL_WOOD,
			WELL_FULL,
			WELL_EMPTY,
			FOOD_GORE_2, // Tasty blood.
			FLAG,
			TRASH_1, // Previously the Call Apogee sign
			TRASH_2,
			TRASH_3,
			TRASH_4,
			PANS_2,
			STOVE,
			SPEARS,
			VINES
		};
		struct Sprite {
			coord x;
			coord y;
			StaticSprite spr;
		};
		static const int Max    = 48;              // There are 48 static sprites in wolf3d
		static const int Start  = 2;               // First file is 002.tga
		static const int End    = Start + Max - 1; // Last file is 049.tga
		static const word First = 23;              // First sprite block number is 0x17
		static const word Last  = First + Max - 1; // Last is 0x46

		static inline StaticSprite ToSprite(const word data) {
			return static_cast<StaticSprite>(data - First);
		}
	}

	namespace EnemySprites {
		static const word GuardStart	= 54;
		static const word GuardEnd		= 102;
		// This is a lie. The dogs lack idle animations, so we fake them.
		static const word DogStart		= 95;
		static const word DogEnd		= 141;
		static const word OfficerStart	= 142;
		static const word OfficerEnd	= 190;
		static const word MutantStart	= 191;
		static const word MutantEnd		= 241;
		static const word SSStart		= 242;
		static const word SSEnd			= 291;
	}

	static const word NumAreas   = 0x25;
	static const word FirstArea  = 0x6B; // First area is always secret area.
	static const word LastArea   = FirstArea + NumAreas - 1;
	static const word AmbushTile = 0x6A;
	static const word FirstWall  = 1;
	static const word LastWall   = AmbushTile - 1;
}
