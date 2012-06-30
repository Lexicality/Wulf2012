#version 150
#extension GL_ARB_explicit_attrib_location : require
//
// WulfGame/OpenGL/Shaders/GenericAI.vert
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

layout (location = 0) in vec2  gridPos;
layout (location = 1) in float heading;
layout (location = 2) in int   activity;

flat out int spriteNum;

uniform vec4 PlayerPosition;

const float pi = 3.1415;
const float pi1_8 = pi * 1 / 8;
const float pi3_8 = pi * 3 / 8;
const float pi5_8 = pi * 5 / 8;
const float pi7_8 = pi * 7 / 8;

float angle_clamp(float ang);

void main()
{
	gl_Position = vec4(gridPos,0,1);
	vec2 npos = gridPos - PlayerPosition.xy;
	float angrel = angle_clamp(atan(npos.y, npos.x) + heading);
	if (angrel < pi1_8) {
		if (angrel > -pi1_8)
			spriteNum = 0;
		else if (angrel > -pi3_8)
			spriteNum = 1;
		else if (angrel > -pi5_8)
			spriteNum = 2;
		else if (angrel > -pi7_8)
			spriteNum = 3;
		else
			spriteNum = 4;
	} else {
		if (angrel > pi7_8)
			spriteNum = 4;
		else if (angrel > pi5_8)
			spriteNum = 5;
		else if (angrel > pi3_8)
			spriteNum = 6;
		else
			spriteNum = 7;
	}
}

float angle_clamp(float ang)
{
    if (ang > pi)
        return ang - 2 * pi;
    else if (ang < -pi)
        return ang + 2 * pi;
    else
        return ang;
}
