//
// WulfGame/OpenGL/Renderer.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "OpenGL/Renderer.h"

#include <GL/glfw.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <algorithm>
#include <functional>
#include <iostream>
#include "Map/Constants.h"

void errchck(const char* str);

using namespace Wulf;
namespace arg = std::placeholders; // whee

// Statix
#ifndef FREE_VIEW
static const glm::vec3 vPlyUp(0, 0, 1);
#endif

#ifdef _WIN32
// and fuck you too windows! (Enable sprintf)
#pragma warning( disable : 4996 )
#endif

void OpenGL::Renderer::WallsRenderChunk::mRenderFunction(const OpenGL::RenderChunk&) const
{
	glMultiDrawArrays(GL_TRIANGLE_FAN, &(*vFirsts)[0], &(*vCounts)[0], NumObjs);
}

void GenericRenderFunction (const OpenGL::RenderChunk& self)
{
	glDrawArrays(self.DrawMode, 0, self.NumObjs);
}

OpenGL::Renderer::Renderer()
: mgr(), fnt(mgr), strbuff(nullptr)
{
	/*
		A lot of the boilerplate code in this function is originally
		 from the fine http://opengl-tutorial.org website.
		Happily, it's all released under the WTFPL license, so no worries.
	*/

	// Initialize GLFW
	if( !glfwInit() )
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}

	// From this point on, we need to kill GLFW if anything else fails.
	// Therefore we delay any exceptions for a moment to do so.
	try {
	
		glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
		glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE,GL_TRUE);
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
		glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
		// Open a window and create its OpenGL context
		// TODO: Config!

        windowWidth  = 1024;
        windowHeight = 768;
        //viewportHeight = 614; // 5:3 aspect ratio like original game
        //hudHeight = windowHeight - viewportHeight;
        
        // The hud is 1/6 of the window height
        const double ratio = 1.0 / 6.0;
        hudHeight = (static_cast<double>(windowHeight) * ratio);
        // Our unauthentic borderless viewport gets everything left.
        viewportHeight = windowHeight - hudHeight;
		if( !glfwOpenWindow( windowWidth, windowHeight, 0,0,0,0, 32,0, GLFW_WINDOW ) )
		{
			throw std::runtime_error("Failed to open GLFW window");
		}
		
		// TODO: Maybe FoV config?
		projectionMatrix = glm::perspective(75.0f, // FoV
			static_cast<GLfloat> (windowWidth) /   // Aspect ratio
			static_cast<GLfloat> (viewportHeight),
			0.1f, 100.0f);                         // nearZ, farZ
	
#ifndef __APPLE__
		// Do some GLEW now we've got a context up.
		glewExperimental = GL_TRUE;
		GLenum glewStatus = glewInit();
		
		if (glewStatus != GLEW_OK) {
			std::string serr = "Failed to initialize GLEW: ";
			// I don't like using the forceful C cast but none of the c++ casts
			//  would let this happen. >:(
			serr += (const char*) glewGetErrorString(glewStatus);
			throw std::runtime_error(serr);
		}
		if (!GLEW_VERSION_3_2) {
			throw std::runtime_error("OpenGL 3.2 is not supported here!");
		}
#endif
		
		glfwDisable(GLFW_MOUSE_CURSOR);

		glfwSetWindowTitle( "Wulf2012" );

		// Vshink
		//glfwSwapInterval(1);
		
	
		// Ensure we can capture the escape key being pressed below
		//glfwEnable( GLFW_STICKY_KEYS );
	
		// Dark blue background
		glClearColor(0.0f, 0.0f, 0.3f, 0.0f);
	
		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS); 
	
		// Alpha
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //#define TRIANGLE_DEBUGGING
#ifdef TRIANGLE_DEBUGGING
#pragma message("Triangle debugging mode is active!")
		// Debug shit
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

        //#define NO_TRIANGLE_CULLING
#ifdef NO_TRIANGLE_CULLING
#pragma message("Backface culling is NOT ACTIVE!")
		glDisable(GL_CULL_FACE);
#else
		// Cull triangles whose normals are not facing towards the camera
		glEnable(GL_CULL_FACE);
#endif
		// Store our render chunks 
		AddRenderChunk(&Walls);
        AddRenderChunk(&Floor);
        AddRenderChunk(&Statics);
		AddRenderChunk(&Doors);

		// Do the render chunks
        Walls.RenderFunction = std::bind(&WallsRenderChunk::mRenderFunction, &Walls, arg::_1);
		Floor.RenderFunction   = std::bind(GenericRenderFunction, arg::_1);
		Doors.RenderFunction   = std::bind(GenericRenderFunction, arg::_1);
		Statics.RenderFunction = std::bind(GenericRenderFunction, arg::_1);
		
		Floor.DrawMode = GL_TRIANGLE_FAN;
		Doors.DrawMode = GL_POINTS;
		Statics.DrawMode = GL_POINTS;
        
        Walls.vFirsts = &vFirsts;
        Walls.vCounts = &vCounts;

		// Load the VBOs
		std::vector<GLuint> VBOs = mgr.CreateVBOs(5);
		Walls.VBO   = VBOs[0];
		Floor.VBO   = VBOs[1];
		Statics.VBO = VBOs[2];
		Doors.VBO   = VBOs[3];
		// Such gluttony, demanding two vbos!
        Doors.Other["VBO2"] = VBOs[4];
	
		// Then the VAOs that use the VBOs (even if they don't use them yet)
		std::vector<GLuint> VAOs = mgr.CreateVAOs(4);
		Walls.VAO   = VAOs[0];
		Floor.VAO   = VAOs[1];
		Statics.VAO = VAOs[2];
		Doors.VAO   = VAOs[3];
		
		
		// Set them up
		glBindVertexArray(Walls.VAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	
		glBindVertexArray(Floor.VAO);
		glEnableVertexAttribArray(0);

		glBindVertexArray(Statics.VAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(Doors.VAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
	
		glBindVertexArray(0);

		LoadShaders();

		// All textures and shit
		glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
		glEnable(GL_PROGRAM_POINT_SIZE);
		// Walls
		glActiveTexture(GL_TEXTURE0);
		mgr.LoadTextureArray("walls");
		
		// Static sprites
		glActiveTexture(GL_TEXTURE1);
		mgr.LoadTextureArray("sprites", StaticSprites::Start, StaticSprites::End);





#ifdef FREE_VIEW
		vPlyUp = glm::vec3(0, 0, 1);
#endif

		// Fonts
		fnt.Initialize(20);
        
        // Hud
        hud.Setup(mgr, 25);
		
		// Timing
		ltime = glfwGetTime();

		// misc
		strbuff = new char[26];
		memset(strbuff, 0, 26);
		
		errchck("Post context startup");
	
		// Woo, finally done!
	} catch (...) {
		// Always clean up :D
		glfwTerminate();
		delete[] strbuff;
		throw;
	}

}

OpenGL::Renderer::~Renderer()
{
	glfwTerminate();
	delete strbuff;
}






double OpenGL::Renderer::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 mView = getViewMatrix();
    
    glViewport(0, hudHeight, windowWidth, viewportHeight);
    
	errchck("pre render");
	std::for_each(chunks.begin(), chunks.end(), [&mView](RenderChunk * rndr) {
		rndr->Render(mView);
	});
	errchck("post render");
    
    glViewport(0, 0, windowWidth, hudHeight);
    
	errchck("pre hud");
    hud.Draw();
	errchck("post hud");
    
    glViewport(0, 0, windowWidth, windowHeight);


	// Position/Angle debug
	//sprintf(strbuff, "Pos: x: % 05.2f y: % 05.2f", vPlyPos.x, vPlyPos.y);
	//fnt.DrawStringXY(10, 40, strbuff);
	//sprintf(strbuff, "Dir: x: % 05.2f y: % 05.2f", vPlyDir.x, vPlyDir.y);
	//fnt.DrawStringXY(10, 60, strbuff);


	double ctime, dtime;
	// Time it took for this frame to render. (Informative for perf but useless)
	ctime = glfwGetTime();
	dtime = ctime - ltime;
	sprintf(strbuff, "Render Time: %05.3fms", dtime * 1000);
	errchck("pre font");
	fnt.DrawStringXY(10, 20, strbuff);
	errchck("post font");
	
	glfwSwapBuffers();
    errchck("Post buffer swap (aka broken by OpenGL Profiler!)");

	// Time since last frame (Actually useful)
	ctime = glfwGetTime();
	dtime = ctime - ltime;
	ltime = ctime;

	// Temporary spasticated door slamming to show off doors.
	// (Should not be here but I needed ctime)
	UpdateDoor(11, static_cast<byte>(std::floor((std::sin(ctime) + 1) * 50)));

	return dtime;
}








void OpenGL::Renderer::SetMap(const Map::Map& map)
{
	/*
	 * WALLS
	 */
	std::vector<glm::vec3> walldata;
	GLuint iNumWalls = map.GetPackedQuads(walldata);
	Walls.NumObjs = iNumWalls;

	// Simple, 4 vertexes per wall
	vCounts.assign(iNumWalls, 4);
	
	// slightly trickier
	vFirsts.clear();
	vFirsts.reserve(iNumWalls);
	for (unsigned int i = 0; i < iNumWalls; ++i)
		vFirsts.push_back(i * 4);

	// Now the data
	glBindVertexArray(Walls.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, Walls.VBO);
	GLsizeiptr wallssize = sizeof(glm::vec3) * walldata.size();
	glBufferData(GL_ARRAY_BUFFER, wallssize, &walldata[0], GL_STATIC_DRAW);

	// Note: The data is two sets of 3 floats per point.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, // standard xyz vertexes
		sizeof(GLfloat) * 3 * 2, // Packed in two blocks of three floats
		reinterpret_cast<void*> (0)); // The position is the first bit of data

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, // Standard stp texture coords
		sizeof(GLfloat) * 3 * 2, // Packed in two blocks of three floats
		reinterpret_cast<void*> (sizeof(GLfloat) * 3)); // The texcoord is the second bit of data

	
	/*
	 * STATIC POINT SPRITES
	 */
	std::vector<short int> spritedata;
	GLuint iNumStatix = map.GetPackedSprites(spritedata);
	Statics.NumObjs = iNumStatix;

	std::size_t size = sizeof(short int);

	// Happily this is far simpler.
	glBindVertexArray(Statics.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, Statics.VBO);
	GLsizeiptr spritessize = size * spritedata.size();
	glBufferData(GL_ARRAY_BUFFER, spritessize, &spritedata[0], GL_STATIC_DRAW);

	glVertexAttribPointer (0, 2, GL_SHORT, GL_FALSE, size * 3, reinterpret_cast<void*>(0));
	glVertexAttribIPointer(1, 1, GL_SHORT, size * 3, reinterpret_cast<void*>(size * 2));

	
	/*
	 * DOORS
	 */
	std::vector<short int> doordata;
	GLuint iNumDoors = map.GetPackedDoors(doordata);
	Doors.NumObjs = iNumDoors;
	glBindVertexArray(Doors.VAO);
	GLsizeiptr doorsize = size * doordata.size();

	// Standard static door info
	glBindBuffer(GL_ARRAY_BUFFER, Doors.VBO);
	glBufferData(GL_ARRAY_BUFFER, doorsize, &doordata[0], GL_STATIC_DRAW);
	// Pointahs
	// Position
	glVertexAttribPointer (0, 2, GL_SHORT, GL_FALSE, size * 4, reinterpret_cast<void*>(       0));
	// Texture
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_SHORT,	 size * 4, reinterpret_cast<void*>(size * 2));
	// Direction. This should be a bool. It's not.
	glVertexAttribIPointer(2, 1, GL_SHORT,			 size * 4, reinterpret_cast<void*>(size * 3));

	// Data updated erry time a door moves.
	std::vector<byte> dummydata(iNumDoors, 0);
	glBindBuffer(GL_ARRAY_BUFFER, Doors.Other["VBO2"]);
	glBufferData(GL_ARRAY_BUFFER, iNumDoors, &dummydata[0], GL_DYNAMIC_DRAW);
	// Open percentage, 0->100.
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, 0, 0);

	/*
	 * CEILING
	 */
	// Get the ceiling's colour
	const glm::vec3 c = map.ceilingColour;
	// Since we don't actually have a ceiling,
	//  set the render background so it looks like we do.
	glClearColor(c.r, c.g, c.b, 0);

	/*
	 * FLOOR
	 */
	// Get the floor's colour
	const glm::vec3 f = map.floorColour;
	// Uniforms stay uniform through different programs being active.
	glUseProgram(Floor.Program);
	GLuint ColourLoc = Floor.Other["Colour"];
	glUniform3f(ColourLoc, f.r, f.g, f.b);
	glUseProgram(0);

	// TODO: The map is ALWAYS 64x64, so this only needs to happen once.
	// Generate a new floor that matches the map's dimensions
	Floor.NumObjs = 4;
	const GLfloat w = static_cast<GLfloat> (map.width ) / 2 + 1;
	const GLfloat h = static_cast<GLfloat> (map.height) / 2 + 1;
	const GLfloat floor[] = {
		 w,      h - 1,
		-w + 2,  h - 1,
		-w + 2, -h + 1,
		 w,     -h + 1
	};
	glBindVertexArray(Floor.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, Floor.VBO);
	// This is a normally packed array of 4 xyz vertexes.
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 4, floor, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*> (0));

	// done
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// privates
glm::mat4 OpenGL::Renderer::getViewMatrix() const
{
	return viewMatrix;
}

bool OpenGL::Renderer::IsWindowStillOpen() const
{
	return glfwGetWindowParam(GLFW_OPENED) == GL_TRUE;
}

void OpenGL::Renderer::LoadShaders()
{
#ifdef SHADER_DEBUG
	std::cout <<
		"---------------------\n"
		"Loading Shaders\n"
		"---------------------" << std::endl;
#endif // DEBUG
	// Load our shaders
	// This is a bit silly, but I don't want to refactor this code.
	GLuint& WallsProg = Walls.Program;
	GLuint& FloorProg = Floor.Program;
	GLuint& DoorsProg = Doors.Program;
	GLuint& StatixProg = Statics.Program;
	// Original function
	WallsProg  = mgr.LoadShaders("Walls",                 "MultiTextured");
	DoorsProg  = mgr.LoadShaders("Doors",     "Doors",    "MultiTextured");
	StatixProg = mgr.LoadShaders("Statics",   "BBSprite", "MultiTextured");
	FloorProg = mgr.LoadShaders("Floor", "SolidColour");

	// Fling uniform uniforms at them
    Floor.ViewUniform     = glGetUniformLocation(FloorProg, "View");
    Floor.Other["Colour"] = glGetUniformLocation(FloorProg, "Colour");
	// woo view
	Walls.ViewUniform   = glGetUniformLocation(WallsProg,  "View");
	Doors.ViewUniform   = glGetUniformLocation(DoorsProg,  "View");
	Statics.ViewUniform = glGetUniformLocation(StatixProg, "View");

	// Uniform Uniforms
	GLuint progs[] = {WallsProg, StatixProg, DoorsProg, 0};
	GLuint texes[] = {0, 1, 0};
	int i = 0;
	GLuint prog;
	// I dunno what type this actually is lol
	const auto& ptr = glm::value_ptr(projectionMatrix);
	while ((prog = progs[i]) != 0) {
		glUseProgram(prog);
		glUniform1i(glGetUniformLocation(prog, "Texture"), texes[i]);
		glUniformMatrix4fv(glGetUniformLocation(prog, "Projection"), 1, GL_FALSE, ptr);
		++i;
	}

	// Floor
    glUseProgram(FloorProg);
    glUniformMatrix4fv(glGetUniformLocation(FloorProg, "Projection"), 1, GL_FALSE, ptr);
	
	// Done
	glUseProgram(0);
}

void OpenGL::Renderer::UpdatePlayerInfo(const Player& ply)
{
    hud.UpdatePlayerInfo(ply);
	vPlyPos = ply.GetPos();
	vPlyDir = ply.GetHeading();
#ifdef FREE_VIEW
	vPlyUp  = ply.GetUp();
#endif
	viewMatrix = glm::lookAt(vPlyPos, vPlyPos + vPlyDir, vPlyUp);
}

void OpenGL::Renderer::UpdateDoor(word doornum, byte openPercent)
{
	glBindBuffer(GL_ARRAY_BUFFER, Doors.Other["VBO2"]);
	glBufferSubData(GL_ARRAY_BUFFER, doornum, 1, &openPercent);
}

void OpenGL::Renderer::AddRenderChunk(RenderChunk *chunk)
{
	chunks.push_back(chunk);
}

void OpenGL::Renderer::AddRenderChunk(HasRenderChunk& chunkable)
{
	AddRenderChunk(chunkable.GetRenderChunk(mgr, projectionMatrix));
}

void OpenGL::Renderer::AddRenderChunk(std::function<RenderChunk*(ResourceManager&, glm::mat4 const&)> chunkfunc)
{
    AddRenderChunk(chunkfunc(mgr, projectionMatrix));
}


void errchck(const char* str)
{
	//GLenum err = glGetError();
	//if (err != GL_NO_ERROR) {
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "fuckup in " << str << std::endl;
		if (err == GL_INVALID_ENUM)
			std::cerr << "INVALID ENUM!" << std::endl;
		else if (err == GL_INVALID_VALUE)
			std::cerr << "Invalid value!" << std::endl;
		else if (err == GL_INVALID_OPERATION)
			std::cerr << "Invalid operation!" << std::endl;
		else if (err == GL_INVALID_FRAMEBUFFER_OPERATION)
			std::cerr << "whoops, there goes the framebuffer" << std::endl;
		else if (err == GL_OUT_OF_MEMORY)
			std::cerr << "ABORT ABORT" << std::endl;
	}
}