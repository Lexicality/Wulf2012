//
// WulfGame/Map/Constants.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include "WulfConstants.h"
#include "Map/Sprites.h"

namespace Wulf {
	static const word NumAreas   = 0x25;
	static const word FirstArea  = 0x6B; // First area is always secret area.
	static const word LastArea   = FirstArea + NumAreas - 1;
	static const word AmbushTile = 0x6A;
	static const word FirstWall  = 1;
	static const word LastWall   = AmbushTile - 1;
}
