//
// WulfGame/OpenGL/FontRenderer.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include "OpenGL/ResourceManager.h"

namespace Wulf {
	namespace OpenGL {
		enum class Font {
			Normal = 0,
			Bold
		};
		class FontRenderer {
		public:
			FontRenderer(ResourceManager& mgr);
			~FontRenderer();
			void Initialize(const GLuint textureOffset = 10);
			void DrawString(const GLfloat x, const GLfloat y, const char *words, const Font font = Font::Normal);
			void DrawStringXY(const GLuint  x, const GLuint  y, const char *words, const Font font = Font::Normal);

		private:
			ResourceManager& mgr;

			std::vector<GLuint> programs;
			std::vector<char>   charNums;
			std::vector<GLuint> charPosses;
			std::vector<GLuint>  posPosses;
			std::vector<GLuint> scalePosses;

			// Aaaaaaaaaaaaaaaaaaaaa
			std::vector<std::vector<GLfloat>> charWidths;

			// Avoid recreating shit
			std::vector<GLubyte> chars;
			std::vector<GLfloat> poses;
			GLfloat width;
			char mchars;
			byte cchar;

			// OGL
			GLuint vao;
			GLuint charvbo, posvbo;

			// Viewport tinks
			GLfloat vWidth;
			GLfloat vHeight;
			GLfloat hvWidth;
			GLfloat hvHeight;
		};
	}
}
