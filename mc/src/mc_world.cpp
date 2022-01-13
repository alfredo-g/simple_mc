inline Chunk*
GetChunk(World_Map* world, i32 x, i32 z) {
    
    for(i32 index = 0; index < (i32)(world->ChunkCount*world->ChunkCount); index++) {
        Chunk* chunk = world->Chunks + index;
        
        if(chunk->WorldPosition.x == x && chunk->WorldPosition.y == z) {
            return chunk;
        }
    }
    
    return 0;
}

inline Block
Chunk_GetBlock(World_Map* world, Chunk* chunk, i32 x, u32 y, i32 z) {
    Block result = {};
    const u32 dimX = world->ChunkDim;
    const u32 dimY = world->ChunkDimY;
    const u32 dimZ = dimX;
    
    if(!chunk || !chunk->Blocks) return result;
    
#if 1
    // if we out of chunk bounds...
    // TODO: Change this
    //if(x == (i32)dimX-1) x++;
    //if(z == (i32)dimZ-1) z++;
    
    if(x < 0 || z < 0 || (u32)x >= dimX || (u32)z >= dimZ) {
        v2i chunkPos;
        v3i blockPos;
        if(x < 0) {
            chunkPos = v2i{(i32)chunk->WorldPosition.x + x, (i32)chunk->WorldPosition.y};
            blockPos = v3i{(i32)dimX+x, (i32)y, z};
        } else if(z < 0) {
            chunkPos = v2i{(i32)chunk->WorldPosition.x, (i32)chunk->WorldPosition.y + z};
            blockPos = v3i{x, (i32)y, (i32)dimZ+z};
        } else if((u32)x >= dimX) {
            chunkPos = v2i{(i32)chunk->WorldPosition.x + (i32)((dimX+1)-x), (i32)chunk->WorldPosition.y};
            blockPos = v3i{(i32)(dimX-x), (i32)y, z};
            Assert(blockPos.x == 0);
        } else {
            chunkPos = v2i{(i32)chunk->WorldPosition.x, (i32)chunk->WorldPosition.y + (i32)((dimZ+1)-z)};
            blockPos = v3i{x, (i32)y, (i32)(dimZ-z)};
            Assert(blockPos.z == 0);
        }
        
        
        // Get the neighbor chunk
        Chunk* newChunk = GetChunk(world, chunkPos.x, chunkPos.y);
        // TODO: Do it without recursion
        result = Chunk_GetBlock(world, newChunk, blockPos.x, blockPos.y, blockPos.z);
        
        return result;
    } 
    
    if(y < dimY) {
        result = chunk->Blocks[z*dimY*dimX + y*dimX + x];
    }
#else 
    if(x < 0 || z < 0 || (u32)x >= dimX || (u32)z >= dimZ) return result;
    
    if(y < dimY) {
        result = chunk->Blocks[z*dimY*dimX + y*dimX + x];
    }
#endif
    
    return result;
}

internal void
Chunk_GenerateNew(Chunk* chunk, u32 chunkDim, u32 chunkDimY, i32 seed) {
    const u32 chunkDimZ = chunkDim;
    chunk->Blocks = (Block*)calloc(chunkDim * chunkDimY * chunkDimZ, sizeof(Block));
    i32 chunkPosX = chunk->WorldPosition.x*chunkDim;
    i32 chunkPosZ = chunk->WorldPosition.y*chunkDimZ;
    
    SimplexNoise generator = SimplexNoise();
    const f32 incrementSize = 1000.0f;
    
    for (u32 y = 0; y < chunkDimY; y++) {
        for (u32 z = 0; z < chunkDim; z++) {
            for (u32 x = 0; x < chunkDim; x++) {
                Block* cube = chunk->Blocks + z*chunkDimY*chunkDim + y*chunkDim + x;
                
                u16 maxHeight = (u16)(MapRange(generator.fractal(7, 
                                                                 (x + chunkPosX + seed) / incrementSize, 
                                                                 (z + chunkPosZ + seed) / incrementSize),
                                               -1.0f, 1.0f, 0.0f, 1.0f) * (f32)(chunkDimY-1));
                u16 stoneHeight = (u16)(MapRange(generator.fractal(7, 
                                                                   (x + chunkPosX) / incrementSize, 
                                                                   (z + chunkPosZ) / incrementSize),
                                                 -1.0f, 1.0f, 0.0f, 1.0f) * (f32)((chunkDimY-1)));
                stoneHeight = (stoneHeight * maxHeight) / 127;
                
                if(y == 0) { // Bottom of the chunk
                    cube->ID = BlockID_Bedrock;
                } else if(y == maxHeight) { // Top of the chunk
                    cube->ID = BlockID_Grass;
                } else if(y < stoneHeight) {
                    cube->ID = BlockID_Stone;
                } else if(y < maxHeight) { 
                    cube->ID = BlockID_Dirt;
                }
            }
        }
    }
}

internal void
World_GenerateMap(World_Map* world) {
    
    srand((u32)time(0));
    world->WorldGenSeed = world->WorldGenSeed ? world->WorldGenSeed : (i32)(((f32)rand() / (f32)RAND_MAX) * 1000.0f);
    printf("Seed: %d\n", world->WorldGenSeed);
    
    for(i32 z = 0; z < (i32)world->ChunkCount; ++z) {
        for(i32 x = 0; x < (i32)world->ChunkCount; ++x) {
            Chunk* chunk = world->Chunks + z*world->ChunkCount + x;
            chunk->WorldPosition = v2i{x, z};
            
            Chunk_GenerateNew(chunk, world->ChunkDim, world->ChunkDimY, world->WorldGenSeed);
        }
    }
}