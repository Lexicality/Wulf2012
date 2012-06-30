#version 150
#extension GL_ARB_explicit_attrib_location : require
//
// WulfGame/OpenGL/Shaders/Floor.vert
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

layout (location = 0) in vec2 pos;

uniform mat4 Projection;
uniform mat4 View;

void main() {
	gl_Position =  Projection * View * vec4(pos, 0, 1);
}

