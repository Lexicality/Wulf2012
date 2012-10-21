//
// WulfGame/OpenGL/HUDRenderer.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

#include "OpenGL/HUDRenderer.h"

using namespace Wulf::OpenGL;

HUDRenderer::HUDRenderer()
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
    glBindTexture(GL_TEXTURE, mgr.LoadSingleTexture("pics/STATUSBARPIC.tga"));
    
    // BG Texture unit
    glUniform1i(glGetUniformLocation(shader, "bgsampler"), bgtex);
    
    
    // All done
    glUseProgram(0);
}

void HUDRenderer::Draw()
{
    glUseProgram(shader);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);
    glUseProgram(0);
}

void HUDRenderer::UpdatePlayerInfo(const Wulf::Player& ply)
{
    // . . .
}