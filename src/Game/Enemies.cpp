//
// WulfGame/Game/Enemies.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "Game/Enemies.h"
#include "Game/Entity.h"
#include "Game/EntityManager.h"

#include "gl.h"

#include <cmath>

/*
 * <INLINE HEADER>
 */

namespace Wulf {
    namespace Enemies {
        namespace Instances {
            class AIEntity : public Wulf::Enemies::Enemy {
            public:
                enum Activity {
                    REST,
                    PATROLING,
                    CHASING,
                    ATTACKING
                };
                AIEntity();
                virtual ~AIEntity() {}
                
                virtual void RestThink(double dtime);
                virtual void PatrolThink(double dtime);
                virtual void ChaseThink(double dtime);
                virtual void AttackThink(double dtime);
                void Think(double dtime);

                virtual void Attack() = 0;
                
                void SetActivity(const Activity newact) { mode = newact; }
            protected:
                Activity  mode;
            };
            class Guard : public AIEntity {
            public:
                Guard();
                virtual ~Guard() {}
                void Attack();
            };
        }
    }
    
    void DoOpenGLStuff(std::vector<Enemies::Enemy*>& Enemies);
    
    static const word StationaryGuard   = 183;
    static const word PatrolGuard       = 187;
    static const word StationaryOfficer = 191;
    static const word PatrolOfficer     = 195;
    static const word StationarySS      = 201;
    static const word PatrolSS          = 205;
    static const word StationaryDog     = 209;
    static const word PatrolDog         = 213;
    static const word StationaryMutant  = 255;
    static const word PatrolMutant      = 259;
    
    typedef Enemies::Instances::AIEntity::Activity AIActivity;
    class EnemySpawner {
    public:
        EnemySpawner(word SpawnNumber, const std::string classname, const AIActivity activity);
        bool CheckSpawn(const Map::Node& node, const Difficulty cdiff, Enemies::Enemy*& spawned) const;
    private:
        Enemies::Enemy* makeone(const Map::Node& node, const word dir) const;
        AIActivity activity;
        std::string classname;
        word HardMax, HardMin;
        word MediMax, MediMin;
        word EasyMax, EasyMin;
    };
}

/*
 * </INLINE HEADER>
 */

using namespace Wulf;

// Globaculars
EntityManager  *g_mgr;
const Map::Map *g_map;
const Player   *g_ply;

std::vector<EnemySpawner> spawners;


void Enemies::RegisterEntities()
{
    g_mgr = &EntityManager::GetInstance();
    g_mgr->RegisterEntity<Instances::Guard>("guard");
    spawners.emplace_back(StationaryGuard, "guard", AIActivity::REST);
    spawners.emplace_back(PatrolGuard, "guard", AIActivity::PATROLING);
}

std::vector<Enemies::Enemy*> Enemies::SpawnRelevent(const Map::Map& map, const Player& ply) 
{
    g_map = &map;
    g_ply = &ply;
    std::vector<Enemy*> ret;
        
    const Difficulty& difficulty = ply.GetDifficulty();
    const auto& nodes = map.nodes;
    Enemies::Enemy *ptr;
    
    // ARGH
	for (auto i1 = nodes.cbegin(), e1 = nodes.cend(); i1 != e1; ++i1) {
        for (auto i2 = i1->cbegin(), e2 = i1->cend(); i2 != e2; ++i2) {
            const auto& node = *i2;
            if (node.metadata == 0)
                continue;
            ptr = nullptr;
            for (auto i3 = spawners.cbegin(), e3 = spawners.cend(); i3 != e3; ++i3) {
                const auto& spawner = *i3;
                if (spawner.CheckSpawn(node, difficulty, ptr)) {
                    if (ptr != nullptr)
                        ret.push_back(ptr);
                    break;
                }
            }
        }
    }
    DoOpenGLStuff(ret);
    return ret;
}
            
Enemies::Instances::AIEntity::AIEntity()
{
    mActivity = 0;
}
            
void Enemies::Instances::AIEntity::RestThink(double dtime)
{
    
}
void Enemies::Instances::AIEntity::PatrolThink(double dtime)
{
    
}
void Enemies::Instances::AIEntity::ChaseThink(double dtime)
{
    
}
void Enemies::Instances::AIEntity::AttackThink(double dtime)
{
    
}
void Enemies::Instances::AIEntity::Think(double dtime)
{
    if (mode == REST)
        RestThink(dtime);
    else if (mode == PATROLING)
        PatrolThink(dtime);
    else if (mode == CHASING)
        ChaseThink(dtime);
    else if (mode == ATTACKING)
        AttackThink(dtime);
}
Enemies::Instances::Guard::Guard()
{
    SetSprite(Enemies::Guard);
}

void Enemies::Instances::Guard::Attack()
{
    // . . .
}

    EnemySpawner::EnemySpawner(word SpawnNumber, const std::string classname, const AIActivity activity)
: activity(activity), classname(classname)
{
    // wolf3dredux_0.01/game/wolf3d/level/wolf_level.c:211
    HardMax = SpawnNumber;
    HardMin = SpawnNumber - 3;
    SpawnNumber -= 36;
    MediMax = SpawnNumber;
    MediMin = SpawnNumber - 3;
    SpawnNumber -= 36;
    EasyMax = SpawnNumber;
    EasyMin = SpawnNumber - 3;
}

bool EnemySpawner::CheckSpawn(const Map::Node& node, const Difficulty cdiff, Enemies::Enemy*& spawned) const
{
    word mob = node.metadata;
    if (mob < EasyMin || mob > HardMax)
        return false;
    if (mob >= HardMin) {
        if (cdiff == Difficulty::HARD)
            spawned = makeone(node, mob - HardMin);
        return true;
    } else if (mob >= MediMin && mob <= MediMax) {
        if (cdiff >= Difficulty::MEDI)
            spawned = makeone(node, mob - MediMin);
        return true;
    } else if (mob <= EasyMax) {
        spawned = makeone(node, mob - EasyMin);
        return true;
    }
    return false;
}

typedef Enemies::Instances::AIEntity BaseEnemy;
Enemies::Enemy* EnemySpawner::makeone(const Wulf::Map::Node &node, const word dir) const
{
    Vector pos(node.x, node.y, 0);
    Vector heading(0);
    if (dir == 0)
        heading.x = 1;
    else if (dir == 1)
        heading.y = -1;
    else if (dir == 2)
        heading.x = -1;
    else
        heading.y = 1;
    BaseEnemy *ent = dynamic_cast<BaseEnemy*>(g_mgr->CreateEntity(classname, pos, heading));
    ent->SetActivity(activity);
    return ent;
}

static const size_t NUM_ENEMIES = 5;
struct EnemyRenderChunk : public OpenGL::RenderChunk
{
    void mRenderFunction(const RenderChunk&) const;
    void Setup(OpenGL::ResourceManager& mgr, glm::mat4 const&);
    
    EnemyRenderChunk();
    ~EnemyRenderChunk();
    
    GLuint PlyPos;
    GLuint TexPos;
    
    GLint  *Starts;
    GLsizei *Counts;
    GLint  *Texes;
};

EnemyRenderChunk::EnemyRenderChunk()
{
    Starts = new GLint[NUM_ENEMIES];
    Counts = new GLsizei[NUM_ENEMIES];
    Texes  = new GLint[NUM_ENEMIES];
}
EnemyRenderChunk::~EnemyRenderChunk()
{
    delete [] Starts;
    delete [] Counts;
    delete [] Texes;
}

void errchck(const char* str);

void EnemyRenderChunk::mRenderFunction(const OpenGL::RenderChunk&) const
{
    errchck("Pre Enemy Rendar");
	const auto& vPlyPos = g_ply->GetPos();
	glUniform4f(PlyPos, vPlyPos.x, vPlyPos.y, vPlyPos.z, 1);
    errchck("Post glUniform4f");
    
    for (int i = 0; i < NUM_ENEMIES; ++i) {
        if (Counts[i] == 0)
            continue;
        glUniform1i(TexPos, Texes[i]);
        errchck("Post glUniform1i");
        glDrawArrays(GL_POINTS, Starts[i], Counts[i]);
        errchck("Post glDrawArrays");
    }
}

template<class C>
void dozero(C *what, size_t amt)
{
    for (size_t i = 0; i < amt; ++i)
        what[i] = 0;
}

void EnemyRenderChunk::Setup(OpenGL::ResourceManager &mgr, glm::mat4 const& projMat)
{

    dozero(Starts, NUM_ENEMIES);
    dozero(Counts, NUM_ENEMIES);
    dozero(Texes,  NUM_ENEMIES);
    
    RenderFunction = std::bind(&EnemyRenderChunk::mRenderFunction, this, std::placeholders::_1);
    
    VAO = mgr.CreateVAO();
    {
        std::vector<GLuint> VBOs = mgr.CreateVBOs(2);
        VBO = VBOs[0];
		Other["VBO2"] = VBOs[1];
    }
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    
    // Shader
	Program = mgr.LoadShaders("GenericAI", "BBSprite", "MultiTextured");
    glUseProgram(Program);
    glUniformMatrix4fv(glGetUniformLocation(Program, "Projection"), 1, GL_FALSE, glm::value_ptr(projMat));
    glUseProgram(0);
    
	ViewUniform = glGetUniformLocation(Program, "View");
	PlyPos      = glGetUniformLocation(Program, "PlayerPosition");
	TexPos      = glGetUniformLocation(Program, "Texture");
    
    /*
     * Textures
     */
    
    // Guards
    Texes[0] = 2;
    glActiveTexture(GL_TEXTURE0 + Texes[0]);
    mgr.LoadTextureArray("sprites", EnemySprites::GuardStart, EnemySprites::GuardEnd);
    // TODO: the others
}

EnemyRenderChunk mChunk;
OpenGL::RenderChunk* Enemies::GetRenderChunk(OpenGL::ResourceManager& mgr, glm::mat4 const& projMat)
{   
    mChunk.Setup(mgr, projMat);
    return &mChunk;
}

inline
float workOutHeading(const glm::vec3& heading)
{
    return std::atan2f(heading.y, heading.x);
}

void Wulf::DoOpenGLStuff(std::vector<Enemies::Enemy*>& enemies)
{
    std::vector<GLfloat> enemydata;
    std::vector<GLubyte> activities;
    
    enemydata.reserve(enemies.size() * 3);
    activities.reserve(enemies.size());
    
    for (auto i = enemies.cbegin(), e = enemies.cend(); i != e; ++i) {
        const auto& enemy = **i;
        const auto& pos = enemy.GetPos();
        enemydata.push_back(pos.x);
        enemydata.push_back(pos.y);
        enemydata.push_back(workOutHeading(enemy.GetHeading()));
        activities.push_back(enemy.GetActivity());
    }
    
	glBindVertexArray(mChunk.VAO);
    size_t size = sizeof(GLfloat);
    // Float data
    glBindBuffer(GL_ARRAY_BUFFER, mChunk.VBO);
    glBufferData(GL_ARRAY_BUFFER, enemydata.size() * size, &enemydata[0], GL_STATIC_DRAW);
    // Grid position
    glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, size * 3, reinterpret_cast<void*>(       0));
    // Heading
    glVertexAttribPointer (1, 1, GL_FLOAT, GL_FALSE, size * 3, reinterpret_cast<void*>(size * 2));
    // Integer data
    glBindBuffer(GL_ARRAY_BUFFER, mChunk.Other["VBO2"]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLubyte) * activities.size(), &activities[0], GL_STATIC_DRAW);
    // Activity number
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, 0, 0);
	mChunk.Counts[0] = enemies.size();
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


