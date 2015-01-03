//
// WulfGame/OpenGL/RenderChunk.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include "WulfConstants.h"
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <functional>
#include "gl.h"

namespace Wulf {
	namespace OpenGL {
		// Super basic chunk o' data class for storing render instances.
		struct RenderChunk {
			std::function<void(const RenderChunk&)> RenderFunction;
			GLuint ViewUniform;
			GLuint Program;
			GLuint VAO;
			GLuint VBO;
			GLuint NumObjs;
			GLenum DrawMode;
			// Kinda hacky, but just all the other info instances want to store.
			std::unordered_map<std::string, GLuint> Other;

			void Render(const glm::mat4& ViewMatrix) const
			{
				glUseProgram(Program);
				glBindVertexArray(VAO);
				glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
				RenderFunction(*this);
			}
		};
	}
}

