#version 150

//
// WulfGame/OpenGL/Shaders/Doors.geom
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

flat in int spriteNum[];
flat in int direction[];
flat in int openPrcnt[];

smooth out vec3 stp;

uniform mat4 Projection;
uniform mat4 View;

struct vertex {
	vec4 pos;
	vec3 stp;
};

void main() {
	// hm hm hm
	vertex vertacies[4];

	// Screen Space
	mat4 ScreenSpace = Projection * View;
	// Scene Space
	vec4 pos = gl_in[0].gl_Position;
	bool dir = direction[0] == 0;
	float open = float(openPrcnt[0]) / 100;

	float offset = 0.5;

	int snum = spriteNum[0];

	// Bottom left corner
	vertacies[0].pos = pos;
	if (dir)
		vertacies[0].pos.x += offset - open;
	else
		vertacies[0].pos.y -= offset - open;
	vertacies[0].pos = ScreenSpace * vertacies[0].pos;
	vertacies[0].stp = vec3(0, 1, snum);

	// Top left corner
	vertacies[1].pos = pos;
	if (dir)
		vertacies[1].pos.x += offset - open;
	else
		vertacies[1].pos.y -= offset - open;
	vertacies[1].pos.z += 1;
	vertacies[1].pos = ScreenSpace * vertacies[1].pos;
	vertacies[1].stp = vec3(0, 0, snum);

	// Bottom right corner
	vertacies[2].pos = pos;
	if (dir)
		vertacies[2].pos.x -= offset + open;
	else
		vertacies[2].pos.y += offset + open;
	vertacies[2].pos = ScreenSpace * vertacies[2].pos;
	vertacies[2].stp = vec3(1, 1, snum);

	// Top right corner
	vertacies[3].pos = pos;
	if (dir)
		vertacies[3].pos.x -= offset + open;
	else
		vertacies[3].pos.y += offset + open;
	vertacies[3].pos.z += 1;
	vertacies[3].pos = ScreenSpace * vertacies[3].pos;
	vertacies[3].stp = vec3(1, 0, snum);

	gl_Position = vertacies[0].pos;
	stp         = vertacies[0].stp;
	EmitVertex();
	gl_Position = vertacies[1].pos;
	stp         = vertacies[1].stp;
	EmitVertex();
	gl_Position = vertacies[2].pos;
	stp         = vertacies[2].stp;
	EmitVertex();
	gl_Position = vertacies[3].pos;
	stp         = vertacies[3].stp;
	EmitVertex();
	// Other side
	gl_Position = vertacies[0].pos;
	stp         = vertacies[0].stp;
	EmitVertex();
	gl_Position = vertacies[1].pos;
	stp         = vertacies[1].stp;
	EmitVertex();
	EndPrimitive();
}
