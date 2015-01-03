//
// WulfGame/OpenGL/FontRenderer.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include "OpenGL/ResourceManager.h"

namespace Wulf {
	namespace OpenGL {
		class FontRenderer {
		public:
			enum Font {
				FONT_NORMAL = 0,
				FONT_BOLD
			};

			FontRenderer(ResourceManager& mgr);
			~FontRenderer();
			void Initialize(const GLuint textureOffset = 10);
			void DrawString(const GLfloat x, const GLfloat y, const char *words, const Font font = FONT_NORMAL);
			void DrawStringXY(const GLuint  x, const GLuint  y, const char *words, const Font font = FONT_NORMAL);

		private:
			ResourceManager& mgr;

			std::vector<GLuint> programs;
			std::vector<char>   charNums;
			std::vector<GLuint> charPosses;
			std::vector<GLuint>  posPosses;

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
			GLfloat hvWidth;
			GLfloat hvHeight;
		};
	}
}
