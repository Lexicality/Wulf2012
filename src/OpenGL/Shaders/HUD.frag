#version 150

//
// WulfGame/OpenGL/Shaders/HUD.frag
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

flat   in int  texunit;
smooth in vec3 stp;

out vec4 fragcolour;

// Texunit uniforms
uniform sampler2D bgsampler;

void drawbg();

void main()
{
    //fragcolour = vec4(1, 0, 0, 1);
    //*
    switch (texunit) {
        case 0:
            // Background
            drawbg();
            break;
            
        default:
            // ????
            fragcolour = vec4(gl_FragCoord.xyz, 1);
            break;
    }
    //*/
}

void drawbg()
{
    fragcolour = texture(bgsampler, stp.st);
}