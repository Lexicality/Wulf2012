#version 150
#extension GL_ARB_explicit_attrib_location : require
//
// WulfGame/OpenGL/Shaders/CollisionDebug.vert
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

layout (location = 0) in vec2 point;

void main() {
	gl_Position = vec4(point / 4, -1, 1);
}
