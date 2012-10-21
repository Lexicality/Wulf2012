#version 150

//
// WulfGame/OpenGL/Shaders/HUD.geom
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

flat out int texunit;
smooth out vec3 stp;

// Various uniforms go here

void processbg();

void main()
{
    processbg();
}

vec4 bgpositions[4] = vec4[](
    vec4(-1,  1, -1, 1), // bottom left
    vec4(-1, -1, -1, 1), // top left
    vec4( 1,  1, -1, 1), // bottom right
    vec4( 1, -1, -1, 1)  // top right
);
vec3 bgstps[4] = vec3[](
    vec3(0, 0, 0),
    vec3(0, 1, 0),
    vec3(1, 0, 0),
    vec3(1, 1, 0)
);

void processbg()
{
    for (int i = 0; i < 4; ++i) {
        gl_Position = bgpositions[i];
        stp = bgstps[i];
        texunit = 0;
        EmitVertex();
    }
    EndPrimitive();
}