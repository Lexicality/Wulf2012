#version 150
#extension GL_ARB_explicit_attrib_location : require
//
// WulfGame/OpenGL/Shaders/Statics.vert
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

layout (location = 0) in vec4 gridPos;
layout (location = 1) in int  spriteNum_in;

flat out int spriteNum;

// simple pass-thru to the geometry magic
void main() {
	gl_Position = gridPos;
	spriteNum = spriteNum_in;
}

