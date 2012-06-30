#version 150

//
// WulfGame/OpenGL/Shaders/BBSprite.geom
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

flat in int spriteNum[];

smooth out vec3 stp;

uniform mat4 Projection;
uniform mat4 View;

void main() {
    // Put us into screen space. 
    vec4 pos = View * gl_in[0].gl_Position;

    int snum = spriteNum[0];

    // Bottom left corner
    gl_Position = pos;
    gl_Position.x += 0.5;
    gl_Position = Projection * gl_Position;
    stp = vec3(0, 1, snum);
    EmitVertex();

    // Top left corner
    gl_Position = pos;
    gl_Position.x += 0.5;
    gl_Position.y += 1;
    gl_Position = Projection * gl_Position;
    stp = vec3(0, 0, snum);
    EmitVertex();

    // Bottom right corner
    gl_Position = pos;
    gl_Position.x -= 0.5;
    gl_Position = Projection * gl_Position;
    stp = vec3(1, 1, snum);
    EmitVertex();

    // Top right corner
    gl_Position = pos;
    gl_Position.x -= 0.5;
    gl_Position.y += 1;
    gl_Position = Projection * gl_Position;
    stp = vec3(1, 0, snum);
    EmitVertex();

    EndPrimitive();
}
