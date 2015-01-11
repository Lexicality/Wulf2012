//
// WulfGame/OpenGL/HUDRenderer.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

#include "OpenGL/HUDRenderer.h"

using namespace Wulf::OpenGL;

HUDRenderer::HUDRenderer()
	: Score(0), Health(0), Ammo(0)
{
	// . . .
}
HUDRenderer::~HUDRenderer()
{
	// . . .
}

void HUDRenderer::Setup(Wulf::OpenGL::ResourceManager& mgr, GLsizei textxureoffset)
{
	VAO = mgr.CreateVAO();

	shader = mgr.LoadShaders("HUD", "HUD", "HUD");

	glUseProgram(shader);

	GLsizei bgtex = textxureoffset;
	// other texes are + etc
	glActiveTexture(GL_TEXTURE0 + bgtex);
	glBindTexture(GL_TEXTURE_2D, mgr.LoadSingleTexture("pics/STATUSBARPIC.tga"));

	// BG Texture unit
	glUniform1i(glGetUniformLocation(shader, "bgsampler"), bgtex);

	// All done
	glUseProgram(0);
}

void HUDRenderer::Draw(FontRenderer& fnt)
{
	glUseProgram(shader);
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
	glUseProgram(0);
	// Ply data
	fnt.DrawString(-0.62f, -0.35f, std::to_string(Score).c_str(), Font::Bold);
	fnt.DrawString(0.35f, -0.35f, std::to_string(Ammo).c_str(), Font::Bold);
	fnt.DrawString(0.1f, -0.35f, std::to_string(Health).c_str(), Font::Bold);
}

void HUDRenderer::UpdatePlayerInfo(const Wulf::Player& ply)
{
	Score = ply.Score;
	Health = ply.Health;
	Ammo = ply.Ammo;
}
