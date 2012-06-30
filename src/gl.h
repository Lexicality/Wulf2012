//
// OpenGL Header Includer
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#ifdef __APPLE__
// OSX already has all the relevent functions since it supports GL3.2 out of the box. (Well, 10.7+)
#define GLFW_INCLUDE_GL3
#include <OpenGL/gl3.h>
// However
#else
// 's a different kettle of fish for everything else, so let GLEW deal with all the function pointers etc.
#include <GL/glew.h>
#endif //__APPLE__
// I don't want this in general so might as well just get rid of it in this header
#define GLFW_NO_GLU
