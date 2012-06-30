#version 150
#extension GL_ARB_explicit_attrib_location : require
//
// WulfGame/OpenGL/Shaders/Font.vert
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

// Str8 passtrhu m8
layout (location = 0) in uint char_in;
layout (location = 1) in float pos_in;

flat out uint  chars;
flat out float poses;

void main() {
	gl_Position = vec4(0,0,0,1);
	chars       = char_in;
	poses		=  pos_in;
}
