#include "Game/Physics/Debugger.h"
#include "Game/Physics/TileData.h"

using namespace Wulf;
using namespace Wulf::Physics;

Debugger::Debugger()
{
}

Debugger::~Debugger()
{
}

// comes from the manager. (This is icky but hey)
CollisionObj getObj(const Entity& ent);

CollisionObj adjust(const CollisionObj& original, Vector centre)
{
	float x = centre.x;
	float y = centre.y;
	CollisionObj ret = {
		original.Left - x,
		original.Top - y,
		original.Right - x,
		original.Bottom - y
	};
	return ret;
}

void add(std::vector<float>& buff, const CollisionObj& obj)
{
	buff.push_back(obj.Left);
	buff.push_back(obj.Bottom);
	buff.push_back(obj.Left);
	buff.push_back(obj.Top);
	buff.push_back(obj.Right);
	buff.push_back(obj.Top);
	buff.push_back(obj.Right);
	buff.push_back(obj.Bottom);
}

// 2 = number of floats/point; 4 = number of points/square
const size_t pointsPerSquare = 2 * 4;
const size_t maxNumSquares = 10;

void Debugger::UpdateScreen(const Entity& ply, const std::vector<TileData const * const>& tiles) const
{
	if (!mRenderChunk.VAO)
		return;
	std::vector<float> buffdata;
	buffdata.reserve(pointsPerSquare * maxNumSquares);
	const Vector centre = ply.GetPos();
	// Woo!~
	add(buffdata, adjust(getObj(ply), centre));
	for (const TileData *const tile : tiles) {
		//if (tile != nullptr && tile->Solid)
		add(buffdata, adjust(tile->Bounds, centre));
	}
	// Adjust the draw calls etc.
	numSquares = buffdata.size() / pointsPerSquare;
	// WOO random opengl
	GLint prevBuff;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prevBuff);
	glBindBuffer(GL_ARRAY_BUFFER, mRenderChunk.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * buffdata.size(), &buffdata[0]);
	glBindBuffer(GL_ARRAY_BUFFER, prevBuff);
}

OpenGL::RenderChunk * Debugger::GetRenderChunk(OpenGL::ResourceManager& mgr, glm::mat4 const&)
{
	// Setup
	mRenderChunk.VAO = mgr.CreateVAO();
	mRenderChunk.VBO = mgr.CreateVBO();
	mRenderChunk.Program = mgr.LoadShaders("CollisionDebug", "SolidColour");
	// We don't use a view uniform
	mRenderChunk.ViewUniform = -1;

	// Programatical stuffs
	glUseProgram(mRenderChunk.Program);
	{
		GLuint colourloc = glGetUniformLocation(mRenderChunk.Program, "Colour");
		auto a = [](int i) -> GLfloat { return static_cast<GLfloat>(i) / 255; };
		glUniform3f(colourloc, a(234), a(92), a(37));
	}
	glUseProgram(0);

	// OpenGL buffery stuffery
	glBindVertexArray(mRenderChunk.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, mRenderChunk.VBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBufferData(GL_ARRAY_BUFFER, pointsPerSquare * maxNumSquares * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// OpenGL render function
	indicies = new GLint[maxNumSquares];
	counts = new GLsizei[maxNumSquares];

	for (int i = 0; i < maxNumSquares; ++i) {
		indicies[i] = i * 4;
		counts[i] = 4;
	}
	mRenderChunk.RenderFunction = [this](const OpenGL::RenderChunk& self) {
		if (numSquares > 0)
			glMultiDrawArrays(GL_LINE_LOOP, indicies, counts, numSquares);
	};

	return &mRenderChunk;
}
