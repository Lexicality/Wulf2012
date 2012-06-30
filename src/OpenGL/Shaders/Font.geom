#version 150

//
// WulfGame/OpenGL/Shaders/Font.geom
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

// Points -> shaped sprites
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

// String
flat in uint  chars[];
flat in float poses[];

// Spr
smooth out vec2 fragST;

// VARIABLE SIZED FONTS ARGH
uniform float widths[127];
uniform vec2  screenScale;
uniform vec2  blockSize;

// Normalised screen position to draw the string from
uniform vec2  pos;

void main() {
	float left   = pos.x + poses[0];
	float bottom = pos.y;
	// Infos
	int char = int(chars[0]);
	float charwidth = widths[char];

	// Posses
	float top   = bottom + blockSize.y * screenScale.y;
	float right = left   + charwidth   * screenScale.x;

	// Texture info
	float row = float(char / 16) * blockSize.y;
	float col = float(char % 16) * blockSize.x;

	// Top Left
	gl_Position = vec4(left, top, -1, 1);
	fragST = vec2(col, row);
	EmitVertex();
    
    // Bottom left
	gl_Position = vec4(left, bottom, -1, 1);
	fragST = vec2(col, row + blockSize.y);
	EmitVertex();
    
	// Top Right
	gl_Position = vec4(right, top, -1, 1);
	fragST = vec2(col + charwidth, row);
	EmitVertex();
    
	// Bottom right
	gl_Position = vec4(right, bottom, -1, 1);
	fragST = vec2(col + charwidth, row + blockSize.y);
	EmitVertex();


	EndPrimitive();
}
