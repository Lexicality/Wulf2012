//
// WulfGame/Game/Player.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "Game/Player.h"

#include <iostream>

using namespace Wulf;

Player::Player()
	: Entity()
#ifdef FREE_VIEW
	, fhViewAngle(0.0f), fvViewAngle(3.14f * 2.0f)
	, mUp(0,0,1)
#else // !FREE_VIEW
	, fViewAngle(0.0f)
#endif // /FREE_VIEW
{
	// Woo.
}

// TODO: Sensitivity settings!
static const float fMouseSpeed = 0.005f;
static const float fKeySpeed   = 3.0f;
#ifdef FREE_VIEW
	static const float clampUp = 3.14f * 2.2f;
	static const float clampDn = 3.14f * 1.8f;
#else
	static const Vector up(0, 0, 1);
#endif

void Player::ProcessUserInput(const Input::Data& input, const double dtime)
{
	// Lose some precision
	const float ftime = static_cast<float>(dtime);

	// Viewage
#ifdef FREE_VIEW
	fhViewAngle -= fMouseSpeed * input.DeltaViewX;
	fvViewAngle -= fMouseSpeed * input.DeltaViewY;
	fvViewAngle = (fvViewAngle > clampUp) ? clampUp : fvViewAngle;
	fvViewAngle = (fvViewAngle < clampDn) ? clampDn : fvViewAngle;

	mDir = glm::vec3(
		sin(fhViewAngle) * cos(fvViewAngle),
		cos(fhViewAngle) * cos(fvViewAngle),
						   sin(fvViewAngle)
	);
	float fhvang = fhViewAngle - 3.14f / 2.0f;
	glm::vec3 right (
		sin(fhvang),
		cos(fhvang),
		0
	);
	mUp = glm::cross(mDir, right);
#else
	fViewAngle -= fMouseSpeed * input.DeltaView;
	mDir = glm::vec3 (
		sin(fViewAngle),
		cos(fViewAngle),
		0
	);
	glm::vec3 right = glm::cross(up, mDir);
#endif
	glm::vec3 vel(0,0,0);
	float amt = ftime * fKeySpeed;
	if (input.Forwards)
		vel += mDir * amt;
	if (input.Backwards)
		vel -= mDir * amt;
	if (input.Left)
		vel += right * amt;
	if (input.Right)
		vel -= right * amt;
#ifdef FREE_VIEW
	mPos += vel;
#else
	this->Move(vel);
#endif
}

void Player::ProcessMapInput(const Map::Map& map)
{
	mPos = map.spawnPos;
#ifdef FREE_VIEW
	fhViewAngle = map.spawnAng;
	fvViewAngle = 3.14f * 2.0f;
	mDir = glm::vec3(
		sin(fhViewAngle) * cos(fvViewAngle),
		cos(fhViewAngle) * cos(fvViewAngle),
						   sin(fvViewAngle)
	);
	float fhvang = fhViewAngle - 3.14f / 2.0f;
	glm::vec3 right (
		sin(fhvang),
		cos(fhvang),
		0
	);
	mUp = glm::cross(mDir, right);
#else // !FREE_VIEW
	fViewAngle = map.spawnAng;
	mDir = glm::vec3 (
		sin(fViewAngle),
		cos(fViewAngle),
		0
	);
#endif // /FREE_VIEW
}
