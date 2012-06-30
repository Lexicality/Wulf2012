#version 150
#extension GL_ARB_explicit_attrib_location : require
//
// WulfGame/OpenGL/Shaders/Doors.Vert
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

layout (location = 0) in vec4 gridPos;
layout (location = 1) in int  spriteNum_in;
layout (location = 2) in int  direction_in;
layout (location = 3) in int  openPrcnt_in;

flat out int spriteNum;
flat out int direction;
flat out int openPrcnt;

// simple pass-thru to the geometry magic
void main() {
	gl_Position = gridPos;
	spriteNum   = spriteNum_in;
	direction   = direction_in;
	openPrcnt   = clamp(openPrcnt_in,0,100);
}

