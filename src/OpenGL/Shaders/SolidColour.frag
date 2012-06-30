#version 150

//
// WulfGame/OpenGL/Shaders/SolidColour.frag
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

uniform vec3 Colour;

out vec4 FragColour;

void main() {
	FragColour = vec4(Colour, 1);
}
