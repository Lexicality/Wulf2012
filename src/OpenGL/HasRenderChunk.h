//
// WulfGame/OpenGL/HasRenderChunk.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include "OpenGL/RenderChunk.h"
#include "OpenGL/ResourceManager.h"

namespace Wulf {
    namespace OpenGL {
        class HasRenderChunk {
        public:
            virtual RenderChunk * GetRenderChunk(ResourceManager& mgr, glm::mat4 const& projMat) = 0;
        protected:
            RenderChunk mRenderChunk;
        };
    }
}
