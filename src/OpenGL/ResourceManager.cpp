//
// WulfGame/OpenGL/ResourceManager.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "OpenGL/ResourceManager.h"
#include <boost/format.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <sstream>
#include "Wrappers/PhysFS.h"
#include "OpenGL/stb_image.h"

#ifdef SHADER_DEBUG
#include <iostream>
#include <sstream>
#endif

using namespace Wulf;

// Image loading routines
int  img_read(void *vstream, char *data, int amt);
void img_skip(void *vstream, unsigned int amt);
int  img_eof (void *vstream);
static const stbi_io_callbacks cbacks = {
	&img_read,
	&img_skip,
	&img_eof
};
struct imgdata {
	byte *data;
	int  width;
	int height;
};

// Locals
void   genericShaderLoad (GLuint shaderID, const std::string& name, const std::string ext);

void errchck(const char* str);

OpenGL::ResourceManager::ResourceManager() {
	// . . .
}
OpenGL::ResourceManager::ResourceManager(ResourceManager&& other)
: vbos(other.vbos), vaos(other.vaos), programs(other.programs), textures(other.textures)
{
	other.vbos.clear();
	other.vaos.clear();
	other.programs.clear();
	other.textures.clear();
}
OpenGL::ResourceManager::~ResourceManager()
{
	if (vbos.size() > 0)
		glDeleteBuffers(vbos.size(), &vbos[0]);
	if (vaos.size() > 0)
		glDeleteVertexArrays(vaos.size(), &vaos[0]);
	if (textures.size() > 0)
		glDeleteTextures(textures.size(), &textures[0]);
	GLuintVector::iterator end = programs.end();
	GLuintVector::iterator itr = programs.begin();
	for (; itr < end; ++itr)
		glDeleteProgram(*itr);
}

GLuint OpenGL::ResourceManager::CreateVBO()
{
	GLuint id;
	glGenBuffers(1, &id);
	vbos.push_back(id);
	return id;
}

GLuintVector OpenGL::ResourceManager::CreateVBOs (GLsizei n)
{
	GLuintVector nbuffers(n);
	glGenBuffers(n, &nbuffers[0]);
	// Local deletion ting
	for (int i = 0; i < n; ++i)
		vbos.push_back(nbuffers[i]);
	return nbuffers;
}

GLuint OpenGL::ResourceManager::CreateVAO()
{
	GLuint id;
	glGenVertexArrays(1, &id);
	vaos.push_back(id);
	return id;
}

GLuintVector OpenGL::ResourceManager::CreateVAOs (GLsizei n)
{
	GLuintVector nbuffers(n);
	glGenVertexArrays(n, &nbuffers[0]);
	// Local deletion ting
	for (int i = 0; i < n; ++i)
		vaos.push_back(nbuffers[i]);
	return nbuffers;
}

GLuint OpenGL::ResourceManager::LoadShaders(const std::string& vertex, const std::string& geometry, const std::string& fragment)
{
	GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
	genericShaderLoad(VertexShader, vertex, ".vert");

	GLuint GeometryShader = 0;
	if (geometry != "") {
		GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		genericShaderLoad(GeometryShader, geometry, ".geom");
	}

	GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	genericShaderLoad(FragmentShader, fragment, ".frag");

	GLuint Program = glCreateProgram();
	glAttachShader(Program, VertexShader);
	if (geometry != "")
		glAttachShader(Program, GeometryShader);
	glAttachShader(Program, FragmentShader);
	glLinkProgram(Program);

	GLint result = GL_FALSE;
	glGetProgramiv(Program, GL_LINK_STATUS, &result);
#ifndef SHADER_DEBUG
	if (result == GL_FALSE)
#endif
	{
		GLsizei len;
		glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &len);
		std::string msg;
        if (!len) {
            msg = "No message.";
        } else {
			std::vector<char> log(len);
			glGetProgramInfoLog(Program, len, NULL, &log[0]);
			msg = &log[0];
		}
#ifdef SHADER_DEBUG
		std::cout << msg << std::endl;
		if (result == GL_FALSE)
#endif
			throw CorruptShaderException(msg);
	}

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
	programs.push_back(Program);

	return Program;
}

GLuintVector OpenGL::ResourceManager::LoadShaders(GLsizei n, const std::string& vertex, const std::string& geometry, const std::string& fragment)
{
	if (n == 1)
		return GLuintVector(1, LoadShaders(vertex, geometry, fragment));
	GLuintVector res;
	res.reserve(n);

	GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
	genericShaderLoad(VertexShader, vertex, ".vert");

	GLuint GeometryShader = 0;
	if (geometry != "") {
		GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		genericShaderLoad(GeometryShader, geometry, ".geom");
	}

	GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	genericShaderLoad(FragmentShader, fragment, ".frag");

	GLint result = GL_FALSE;

	for (GLsizei i = 0; i < n; ++i) {
		GLuint Program = glCreateProgram();
		glAttachShader(Program, VertexShader);
		if (geometry != "")
			glAttachShader(Program, GeometryShader);
		glAttachShader(Program, FragmentShader);
		glLinkProgram(Program);

		glGetProgramiv(Program, GL_LINK_STATUS, &result);
		if (result == GL_FALSE) {
			GLsizei len;
			glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &len);
			std::vector<char> log(len);
			glGetProgramInfoLog(Program, len, NULL, &log[0]);
			std::string msg(&log[0]);
			std::cerr << msg << std::endl;
			throw CorruptShaderException(msg);
		}

		res     .push_back(Program);
		programs.push_back(Program);
	}
#ifdef SHADER_DEBUG
	std::cout << "Successfully linked " << n << " programs with identical shaders." << std::endl;
#endif

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
	return res;
}

void genericShaderLoad (GLuint shaderID, const std::string& name, const std::string ext)
{
	std::string path = "shaders/" + (name + ext);
	if (!PhysFS::exists(path))
		throw OpenGL::NoSuchShaderException("Could not find " + path + "!");

	// http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
	PhysFS::FileStream file(path, PhysFS::OM_READ);
	std::ostringstream buff;
	buff << file.rdbuf();
	file.close();

	std::string src = buff.str();
	const char *csrc = src.c_str();

	glShaderSource(shaderID, 1, &csrc, NULL);
	glCompileShader(shaderID);

	GLint result = GL_FALSE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
#ifndef SHADER_DEBUG
	if (result == GL_FALSE)
#endif
	{
		GLsizei len;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &len);
        std::ostringstream msg;
		msg << name << ext << ":\n";
        if (!len) {
            msg << "No message.";
        } else {
            std::vector<char> log(len);
            glGetShaderInfoLog(shaderID, len, NULL, &log[0]);
            msg << &log[0];
        }
		std::string smsg = msg.str();
#ifdef SHADER_DEBUG
		std::cout << smsg << std::endl;
		if (result == GL_FALSE)
#endif
			throw OpenGL::CorruptShaderException(smsg);
	}
}

GLuint OpenGL::ResourceManager::LoadTextureArray(const std::string& path)
{
	if (!PhysFS::isDirectory(path))
		throw NoSuchTextureException("No such directory " + path + "!");
	auto filesraw = PhysFS::getFileListing(path);
	std::vector<std::string> files;
	files.reserve(filesraw.size());
	for (auto i = filesraw.begin(), e = filesraw.end(); i < e; ++i)
		files.push_back(path + '/' + *i);
	return genericTextureLoad(files);
}
GLuint OpenGL::ResourceManager::LoadTextureArray( const std::string& path, const short int start, const short int end)
{
	// *shrug*
	if (start > end)
		throw NoSuchTextureException("Invalid texture range!");

	if (!PhysFS::isDirectory(path))
		throw NoSuchTextureException("No such directory " + path + "!");

	boost::format filename(path + "/%03d.tga");

	std::vector<std::string> files;
	for (auto i = start; i < end; ++i) {
		std::string file = (filename % i).str();
		if (PhysFS::exists(file))
			files.push_back(file);
	}
	return genericTextureLoad(files);
}

GLuint OpenGL::ResourceManager::LoadSingleTexture(const std::string& path, GLuint *const width, GLuint *const height)
{
	if (!PhysFS::exists(path))
		throw NoSuchTextureException("Could not find " + path);
	imgdata data = {nullptr, 0, 0};

	try {
		int i;
		PhysFS::FileStream file(path, PhysFS::OM_READ);
		// The callbacks may throw an exception if PhysFS messes up.
		data.data = stbi_load_from_callbacks(&cbacks, &file, &data.width, &data.height, &i, 4);
		// C has no such option. Pff
		if (data.data == nullptr)
			throw std::runtime_error(stbi_failure_reason());
	} catch (std::exception& e) {
		throw NoSuchTextureException("Cannot load " + path + ":" + e.what());
	}
	// Helpful stuph
	if (width  != nullptr)
		*width = data.width;
	if (height != nullptr)
		*height = data.height;
	// load it into opengl
	GLuint res;
	glGenTextures(1, &res);
	glBindTexture  (GL_TEXTURE_2D, res);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// Single byte per r,g,b and a.
	GLint prevAlignment;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Slam it inta  a textcha
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, data.width, data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data);

	// Delete the imgdata
	stbi_image_free(data.data);

	// Realign the pixels like they were befo
	glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment);

	// Cleanup
	textures.push_back(res);

	return res;
}

// Image loading routines
int img_read(void *vstream, char *data, int amt)
{
	PhysFS::FileStream *stream = reinterpret_cast<PhysFS::FileStream*>(vstream);
	stream->read(data, amt);
	return static_cast<int>(stream->gcount());
}

void img_skip(void *vstream, unsigned int amt)
{
	reinterpret_cast<PhysFS::FileStream*>(vstream)->ignore(amt);
}

int img_eof(void *vstream)
{
	return reinterpret_cast<PhysFS::FileStream*>(vstream)->eof() ? 1 : 0;
}

GLuint OpenGL::ResourceManager::genericTextureLoad(std::vector<std::string>& files)
{
	std::vector<imgdata> images;
	images.reserve(files.size());

	int i;

	for (auto itr = files.begin(), end = files.end(); itr < end; ++itr) {
		std::string cpath = *itr;
		if (!boost::algorithm::iends_with(cpath, ".tga"))
			continue;
		int x,y;
		byte *data;
		try {
			PhysFS::FileStream file(cpath, PhysFS::OM_READ);
			// The callbacks may throw an exception if PhysFS messes up.
			data = stbi_load_from_callbacks(&cbacks, &file, &x, &y, &i, 4);
			// C has no such option. Pff
			if (data == nullptr)
				throw std::runtime_error(stbi_failure_reason());
		} catch (std::exception& e) {
			std::cerr << "Failed to load " << cpath << ": " << e.what() << std::endl;
			continue;
		}
		imgdata idata = {data, x, y};
		images.push_back(idata);
	}

	if (images.size() < 1)
		throw NoSuchTextureException("No textures found!");

	GLuint res;
	glGenTextures(1, &res);
	glBindTexture(GL_TEXTURE_2D_ARRAY, res);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Single byte per r,g,b and a.
	GLint prevAlignment;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	imgdata *fimg = &images[0];

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, fimg->width, fimg->height,
						images.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	i = 0;
	for (auto itr = images.begin(), end = images.end(); itr < end; ++itr) {
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
			itr->width, itr->height, 1, GL_RGBA, GL_UNSIGNED_BYTE, itr->data);
		stbi_image_free(itr->data);
		++i;
	}

	// Realign the pixels like they were befo
	glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment);

	// Remember to clean up later
	textures.push_back(res);

	return res;
}

GLuint OpenGL::ResourceManager::LoadShaders(const std::string& name)
{
	return LoadShaders(name, "", name);
}

GLuint OpenGL::ResourceManager::LoadShaders(const std::string& vertex, const std::string& fragment)
{
	return LoadShaders(vertex, "", fragment);
}

GLuintVector OpenGL::ResourceManager::LoadShaders(GLsizei n, const std::string& name)
{
	return LoadShaders(n, name, "", name);
}

GLuintVector OpenGL::ResourceManager::LoadShaders(GLsizei n, const std::string& vertex, const std::string& fragment)
{
	return LoadShaders(n, vertex, "", fragment);
}

