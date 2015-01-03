//
// WulfGame/OpenGL/HUDRenderer.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include "WulfConstants.h"
#include "OpenGL/ResourceManager.h"
#include "Game/Player.h"
#include "gl.h"

namespace Wulf {
	namespace OpenGL {
		class HUDRenderer {
		public:
			HUDRenderer();
			~HUDRenderer();

			void Setup(ResourceManager& mgr, GLsizei textureoffset = 15);

			void UpdatePlayerInfo(const Player& ply);

			void Draw();
		private:
			GLuint VAO;
			GLuint shader;
			GLuint fbuff;
		};
	}
}
