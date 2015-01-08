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

#if defined (_MSC_VER) && _MSC_VER < 1700
#error This project needs at least VS2012 to compile!
#endif // _MSC_VER level

namespace Wulf {
	enum Direction {
		North,
		East,
		South,
		West
	};
	typedef unsigned short int Material;
	typedef glm::vec3 Vector;

	typedef unsigned short int EntID;

	typedef uint16_t word;
	typedef uint8_t  byte;

	typedef int16_t coord;
	class coords {
	public:
		coord x, y;

		float distance(coords const pos) const { return distance(pos.x, pos.y); }
		float distance(Vector const pos) const { return distance(pos.x, pos.y); }
		float distance(float px, float py) const
		{
			return glm::distance(glm::vec2(x, y), glm::vec2(px, py));
		}

		coords() : x(0), y(0) {}
		coords(coord x, coord y) : x(x), y(y) {}
		coords(int x, int y) : x(x), y(y) {}
		coords(float x, float y) : x(fromfloat(x)), y(fromfloat(y)) {}
		coords(Vector pos) : x(fromfloat(pos.x)), y(fromfloat(pos.y)) {}

		bool operator==(coords const& o) const { return x == o.x && y == o.y; }
		bool operator!=(coords const& o) const { return !(*this == o); }
		bool operator<(coords const& o) const { return x < o.x && y < o.y; }
	private:
		coord fromfloat(float const num) const
		{
			return static_cast<coord>(std::round(num));
		}
	};
}
namespace std {
	template <> struct hash < Wulf::coords > {
		inline size_t operator() (const Wulf::coords& coords) const
		{
			uint16_t x, y;
			x = static_cast<uint16_t>(coords.x);
			y = static_cast<uint16_t>(coords.y);
			uint32_t pos = (x << 16) | y;
			return std::hash<uint32_t>()(pos);
		}
	};
}
