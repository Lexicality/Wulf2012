//
// WulfGame/Game/Player.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include <glm/glm.hpp>
#include "WulfConstants.h"
#include "Game/InputManager.h"
#include "Game/Constants.h"
#include "Game/Entity.h"
#include "Map/Map.h"

namespace Wulf {
	class Player : public Entity {
	public:
		Player();
		~Player() {};

		void ProcessUserInput(const Input::Data& input, const double dtime);
		void ProcessMapInput(const Map::Map& map);
#ifdef FREE_VIEW
		glm::vec3 GetUp() const { return mUp; }
#endif

		Difficulty GetDifficulty() const { return mDifficulty; }
		void SetDifficulty(Difficulty nDiff) { mDifficulty = nDiff; }

	private:
		Difficulty mDifficulty;
#ifdef FREE_VIEW
		float   fhViewAngle;
		float   fvViewAngle;
		glm::vec3 mUp;
#else
		float   fViewAngle;
#endif
		Player(Player& other);
		Player& operator=(Player& other);
	};
}

