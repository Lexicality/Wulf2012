#version 150

//
// WulfGame/OpenGL/Shaders/MultiTextured.frag
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

smooth in vec3 stp;

out vec4 colour;

uniform sampler2DArray Texture;

void main() {
    // Defeat the evil anti-aliasing monster of doom who leavith bits of sprites where they doth ought n'aer tread.
    if (stp.t < 0 || stp.t > 1)
        discard;
//*
	colour = texture(Texture, stp);
	if (colour.a < 0.2)
		discard;
/*/
	{
		colour.rgb = stp.stp;
		colour.a  = 1;
	}
// */
}
