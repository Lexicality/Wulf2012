//
// WulfGame/OpenGL/FontRenderer.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "OpenGL/FontRenderer.h"
#include <boost/format.hpp>
#include <vector>
#include <stdexcept>
#include "Wrappers/PhysFS.h"

using namespace Wulf;
void errchck(const char* str);

OpenGL::FontRenderer::FontRenderer(OpenGL::ResourceManager& mgr)
	: mgr(mgr)
{
	// . . .
}

OpenGL::FontRenderer::~FontRenderer()
{
	// I'm fairly sure I was supposed to do something here but I'm damned if I know what it was
}

void OpenGL::FontRenderer::Initialize(const GLuint textureOffset)
{
	GLint data[4];
	glGetIntegerv(GL_VIEWPORT, data);
	const GLuint vWidth  = data[2];
	const GLuint vHeight = data[3];
	hvWidth  = static_cast<GLfloat>(vWidth ) / 2.0f;
	hvHeight = static_cast<GLfloat>(vHeight) / 2.0f;

	GLsizei numfonts = 2;
	programs = mgr.LoadShaders(numfonts, "Font", "Font", "Font");
	charWidths.resize(numfonts);
	charPosses.reserve(numfonts);
	posPosses.reserve(numfonts);

	// Woo files
	boost::format fname("pics/font%d.%s");
	for (GLsizei i = 0; i < numfonts; ++i) {
		// First check the dat file
		fname % (i + 1) % "dat";
		std::string datfile = fname.str();
		if (!PhysFS::exists(datfile))
			throw std::runtime_error("Could not find font " + datfile + "!");

		PhysFS::FileStream file(datfile, PhysFS::OM_READ);
		// TODO: maybe I should spin my own error messages.
		file.exceptions(PhysFS::FileStream::eofbit | PhysFS::FileStream::failbit | PhysFS::FileStream::badbit);

		char header[4];
		file.read(header, 4);
		if (!(header[0] == 'f' && header[1] == 'o' && header[2] == 'n' && header[3] == 't'))
			throw std::runtime_error(datfile + " is not a valida font data file!");

		unsigned char blockWidth, blockHeight, numChars;
		file.read((char*)&blockWidth, 1);
		file.read((char*)&blockHeight, 1);
		file.read((char*)&numChars, 1);

		// numChars is stored in a little endian long for no reason.
		// The next 3 chars are always 0 since it's a goddamn char, so ignore 'em
		file.ignore(3);

		// little bit more sanity checking
		if (numChars > 127)
			throw std::runtime_error(datfile + " is not a valida font data file!");
		if (numChars < 96)
			throw std::runtime_error(datfile + " doesn't have enough characters!");

		/*
		 * We in fact remove the first 32 non-printing font characters to save space.
		 * That leaves an extra 31[sic] spaces vacant at the end, which ID apparently filled
		 * With random bits of junk. I'm including them anyway just for the hell of it.
		 */
		charNums.push_back(numChars);

		std::vector<char> widths(numChars);

#ifdef __APPLE__
		// AAAAA
		// std::cout << file.tellg() << std::endl;
		// What the fuck?
		numChars--;
#endif
		file.read(&widths[0], numChars);
		// That's everything from the dat file.
		file.close();

		// Just one problem - These values are in pixels but we work in percents.
		// Let's load the image so we can grab it's dimensions
		fname % (i + 1) % "tga";

		GLuint cprog = programs[i];
		glUseProgram(cprog);
		glActiveTexture(GL_TEXTURE0 + textureOffset + i);

		GLuint iWidth, iHeight;
		GLuint tex = mgr.LoadSingleTexture(fname.str(), &iWidth, &iHeight);
		GLfloat fWidth  = static_cast<GLfloat>(iWidth ),
				fHeight = static_cast<GLfloat>(iHeight);
		glBindTexture(GL_TEXTURE_2D, tex);

		// Texture sampler
		glUniform1i(
			glGetUniformLocation(cprog, "font"),
			textureOffset + i
		);
		// Screen scale
		glUniform2f(
			glGetUniformLocation(cprog, "screenScale"),
			(fWidth  / hvWidth ) * 2,
			(fHeight / hvHeight) * 2
		);
		// Block units
		glUniform2f(
			glGetUniformLocation(cprog, "blockSize"),
			static_cast<GLfloat>(blockWidth ) / fWidth,
			static_cast<GLfloat>(blockHeight) / fHeight
		);
		// Widths
		std::vector<GLfloat>  fwidths;
		std::vector<GLfloat>& swidths = charWidths[i];
		fwidths.reserve(numChars);
		swidths.reserve(numChars);

		std::vector<char>::iterator itr = widths.begin();
		std::vector<char>::iterator end = widths.end();
		for (; itr < end; ++itr) {
			fwidths.push_back(static_cast<GLfloat>(*itr) / fWidth);
			swidths.push_back(static_cast<GLfloat>(*itr) / hvWidth);
		}
		glUniform1fv(
			glGetUniformLocation(cprog, "widths"),
			numChars,
			&fwidths[0]
		);
		// Render-time uniforms
		charPosses.push_back(glGetUniformLocation(cprog, "char"));
		posPosses.push_back(glGetUniformLocation(cprog,  "pos"));
	}

	vao = mgr.CreateVAO();
	auto vbos = mgr.CreateVBOs(2);
	charvbo = vbos[0];
	posvbo  = vbos[1];

	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	/*
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// First, character to draw.
	// Second, screen x pos
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(sizeof(GLubyte)));
	// Done
	*/
}
/*
 * Example usage:
 * fonts.DrawString(-1, -1, "honk", fonts::FONT_BOLD);
 */
void OpenGL::FontRenderer::DrawString(const GLfloat x, const GLfloat y, const char *words, const Font font)
{
	int len = strlen(words);
	if (len == 0)
		return;

	std::vector<GLfloat>& widths = charWidths[font];

	chars.clear();
	poses.clear();
	chars.reserve(len);
	poses.reserve(len);

	width = 0;

	mchars = charNums[font];
	for (int i = 0; i < len; ++i) {
		cchar = words[i] - 32;
		if (cchar < 0 || cchar > mchars)
			continue;
		chars.push_back(cchar);
		// TODO: Change this into a const operation on width.
		poses.push_back(width);
		width += widths[cchar] * 2;
	}

	len = chars.size();

	glUseProgram(programs[font]);

	// Tell the shader where it can stick it
	glUniform2f(posPosses[font], x, y);

	// Build an entirely new buffer to display this text. (woo)
	// TODO: Maybe don't? Have a large GL_DYNAMIC_DRAW buffer and rewrite sections of it.
	//       Pros: Speed
	//       Cons: Length limit. <-- Chars are a fixed (relative) size, so there's a length limit anyway. HMMMMM.
	glBindVertexArray(vao);

	// Chars
	glBindBuffer(GL_ARRAY_BUFFER, charvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLubyte) * len, &chars[0], GL_STREAM_DRAW);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_BYTE, 0, 0);

	// Poses
	glBindBuffer(GL_ARRAY_BUFFER, posvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * len, &poses[0], GL_STREAM_DRAW);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);

	// Now, draw the chars as a great big series of points
	glDrawArrays(GL_POINTS, 0, len);

	glUseProgram(0);
}

void OpenGL::FontRenderer::DrawStringXY(const GLuint  x, const GLuint  y, const char *words, const Font font)
{
	return DrawString(
		(static_cast<GLfloat>(x) - hvWidth ) / hvWidth,
		(hvHeight - static_cast<GLfloat>(y)) / hvHeight,
		words,
		font
	);
}
