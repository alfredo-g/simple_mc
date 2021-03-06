#ifndef MC_H
#define MC_H

#include "mc_platform.h"

struct Camera_View {
    glm::vec3 Position;
    glm::vec3 Orientation;
    glm::vec3 Forward;
    f32 FOV;
};

enum Block_ID{
    BlockID_None,
    BlockID_Grass,
    BlockID_Dirt,
    BlockID_Sand,
    BlockID_Stone,
    BlockID_Bedrock,
    BlockID_GoldOre,
    BlockID_Bookshelf,
    BlockID_DiamondOre,
    BlockID_Rail,
    BlockID_LAST,
};
struct Block {
    Block_ID ID;
    //u8 LightLevel;
    //u8 Rotation;
};
struct Chunk {
    v2i WorldPosition; 
    volatile b32 IsLoaded;
    
    Block* Blocks;
};
struct World_Map {
    u32 ChunkDim;
    u32 ChunkDimY;
    u32 ChunkCount;
    //v2i RenderLowerBound;
    v2i LastPlayerChunkPos;
    u32 RenderDistance; // TODO: Move to Player
    
    i32 WorldGenSeed;
    Chunk* Chunks;
};

struct Game_State {
    World_Map World;
    Memory_Arena WorldArena;
    Camera_View Camera;
    
    Memory_Arena TempArena;
};
#endif //MC_H
