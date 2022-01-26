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
    b32 IsLoaded;
    
    Block* Blocks;
};
struct World_Map {
    u32 ChunkDim;
    u32 ChunkDimY;
    u32 ChunkCount;
    v2i RenderLowerBound;
    v2i RenderUpperBound;
    u32 RenderDistance; // TODO: Move to Player
    
    i32 WorldGenSeed;
    Chunk* Chunks;
};

struct Memory_Arena {
    size_t Used;
    size_t Size;
    u8* Base;
};

struct Game_State {
    World_Map World;
    Memory_Arena WorldArena;
    Camera_View Camera;
    //v2 PlayerPos;
};

internal void
InitArena(Memory_Arena* arena, size_t size, u8* base) {
    arena->Size = size;
    arena->Used = 0;
    arena->Base = base;
}

#define PushStruct(arena, type) (type*)PushSize_(arena, sizeof(type))
#define PushArray(arena, count, type) (type*)PushSize_(arena, (count)*sizeof(type))
internal void*
PushSize_(Memory_Arena* arena, size_t size) {
    Assert(arena->Used + size <= arena->Size);
    void* result = arena->Base + arena->Used;
    arena->Used += size;
    
    return result;
}

#endif //MC_H
