#version 150
#extension GL_ARB_explicit_attrib_location : require
//
// WulfGame/OpenGL/Shaders/Walls.vert
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 UVPv;

uniform mat4 Projection;
uniform mat4 View;

smooth out vec3 stp;

//const vec4 offset = vec4(0.5, -0.5, 0, 0);

void main() {
	//gl_Position =  Projection * View * (offset + vec4(Pos, 1));
	gl_Position =  Projection * View * vec4(Pos, 1);
	stp = UVPv;
}
