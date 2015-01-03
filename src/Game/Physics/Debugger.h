//
// WulfGame/Game/Physics/Debugger.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include <vector>

#include "WulfConstants.h"
#include "Game/Entity.h"
#include "OpenGL/HasRenderChunk.h"

namespace Wulf {
	namespace Physics {
		class TileData;
		class Debugger : public OpenGL::HasRenderChunk {
		public:
			OpenGL::RenderChunk * GetRenderChunk(OpenGL::ResourceManager& mgr, glm::mat4 const&);
			// Graphical Overray
			/*
			This is not actually const. It messes with various C apis to render stuff to
			the screen. However, it needs to be called from a const function so go figure.
			It tries to leave the OGL state as it found it.
			*/
			void UpdateScreen(const Entity& ply, const std::vector<TileData const * const>& tiles) const;
			Debugger();
			~Debugger();
		private:
			mutable GLsizei numSquares;
			GLint *indicies;
			GLsizei *counts;
		};
	}
}
