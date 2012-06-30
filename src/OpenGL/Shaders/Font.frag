#version 150

//
// WulfGame/OpenGL/Shaders/Font.frag
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

smooth in vec2 fragST;

out vec4 color;

uniform sampler2D font;
//uniform int font;

void main() {
	color = texture(font, fragST);
	/*
	if (color.a < 0.5) {
		color.a = 1;
		color.rg = fragST.st;
	}
	// */
}
