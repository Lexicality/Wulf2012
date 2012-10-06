//
// WulfGame/WulfConstants.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include <string>
#include <cstdint>

// GOD DAMN MICROSOFT
#pragma warning( disable : 4250 )

// Woo opengl
#include <glm/glm.hpp>

// Messy but whateverrrr
#define CREATE_EXCEPTION(NAME, DERIVED) class NAME : public DERIVED { public: NAME(const std::string& what) : DERIVED(what) {} }

//#define SHADER_DEBUG

//#define DEBUG_MAP
//#define DEBUG_WALLS_SPAM
//#define DEBUG_HITLER

//#define FREE_VIEW

#ifdef FREE_VIEW
#define NO_TRIANGLE_CULLING
#endif

#if defined (_MSC_VER) && _MSC_VER < 1600
#error This project needs at least VS2010 to compile!
#endif // _MSC_VER level

namespace Wulf {
	enum Direction {
		DIRECTION_NORTH,
		DIRECTION_EAST,
		DIRECTION_SOUTH,
		DIRECTION_WEST
	};
	typedef unsigned short int Material;
	typedef glm::vec3 Vector;

	typedef unsigned short int EntID;

	typedef short int coord;

	typedef uint16_t word;
	typedef uint8_t  byte;
}
