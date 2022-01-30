#include <SimplexNoise.h>

#include "mc.h"
#include "mc_camera.cpp"
#include "mc_world.cpp"
#include "mc_block.cpp"

#if 0
inline b32 
IsPlayerInRenderBounds(v2 playerPos, World_Map* world) {
    b32 result = true;
    
    v2i upper = V2i(playerPos) + (i32)world->RenderDistance;
    v2i lower = V2i(playerPos) - (i32)world->RenderDistance;
    if((upper.x > world->RenderUpperBound.x || upper.y > world->RenderUpperBound.y) ||
       (lower.x < world->RenderLowerBound.x || lower.y < world->RenderLowerBound.y)) {
        result = false;
    }
    
    return result;
}
#endif

inline i32
SearchForChunk(World_Map* world, v2i chunkPos) {
    i32 foundIndex = -1;
    
    for(i32 index = 0; (u32)index < world->ChunkCount*world->ChunkCount; index++) {
        Chunk* chunk = world->Chunks + index;
        
        // In range
        if(chunk->WorldPosition.x == chunkPos.x && chunk->WorldPosition.y == chunkPos.y) {
            foundIndex = index; 
            break;
        } 
    }
    
    return foundIndex;
}


internal void 
LoadVertexBuffer(Open_GL* openGL, Render_Result* render) {
    // Send the data to the GPU
    openGL->SendChunkBuffer(render);
    // Free the vertex memory
    free(render->VertexData);
    render->VertexData = 0;
}

#include "mc_render.cpp"

#ifndef __EMSCRIPTEN__
struct ChunkGen_Data {
    World_Map* World;
    Open_GL* OpenGL;
    i32 ChunkIndex;
    v2i NewPos;
};

internal WORK_QUEUE_CALLBACK(ChunkGenWork) {
    (void)queue;
    ChunkGen_Data* genData = (ChunkGen_Data*)data;
    Chunk* chunk = genData->World->Chunks + genData->ChunkIndex;
    Open_GL* openGL = genData->OpenGL;
    //printf("new Chunk %d\n", genData->ChunkIndex);
    
    chunk->WorldPosition = V2i(genData->NewPos.x, genData->NewPos.y);
    Chunk_GenerateNew(chunk, genData->World->ChunkDim, genData->World->ChunkDimY, genData->World->WorldGenSeed);
    Render_Chunk(&openGL->RenderArena, genData->World, chunk, &genData->OpenGL->RenderData[genData->ChunkIndex]);
    //LoadVertexBuffer(openGL, &openGL->RenderData[genData->ChunkIndex]);
    chunk->IsLoaded = false;
    free(data);
}
#endif


void Game_UpdateAndRender(Game_Memory* gameMemory, Game_Input* input, Open_GL* openGL) {
    Game_State* gameState = (Game_State*)gameMemory->Memory;
    Platform_API* platformAPI = &gameMemory->PlatformAPI;
    Camera_View* camera = &gameState->Camera;
    World_Map* world = &gameState->World;
    
    if(!gameMemory->IsInitialized) {
        
        Block_LoadLocationUV(platformAPI);
        
        InitArena(&gameState->WorldArena, gameMemory->MemorySize - sizeof(Game_State), 
                  (u8*)gameMemory->Memory + sizeof(Game_State));
        InitArena(&openGL->RenderArena, gameMemory->TransientSize, gameMemory->TransientMemory);
        
        //
        // World Init
        world->ChunkDim = 16;
        world->ChunkDimY = 255;
        world->ChunkCount = 7;
        world->RenderDistance = world->ChunkCount/2;
        world->WorldGenSeed = 326;
        world->Chunks = PushArray(&gameState->WorldArena, world->ChunkCount*world->ChunkCount, Chunk);
        
        //
        // Camera Init
        const i32 x = world->ChunkCount/2;
        v2i center = V2i(x, x);
        camera->Position = glm::vec3((f32)(center.x*world->ChunkDim + (world->ChunkDim/2)), 
                                     (f32)world->ChunkDimY-90, 
                                     (f32)(center.y*world->ChunkDim + (world->ChunkDim/2))
                                     );
        camera->FOV = 45.0f;
        camera->Orientation = glm::vec3(0.0f, 0.0f, 0.0f);
        
        // TODO: Take the player initial pos as reference
        world->LastPlayerChunkPos = center;
        
        World_GenerateMap(&gameState->WorldArena, world);
        //openGL->RenderData = (Render_Result*)calloc(world->ChunkCount*world->ChunkCount, sizeof(Render_Result));
        openGL->RenderData = PushArray(&openGL->RenderArena, world->ChunkCount*world->ChunkCount, Render_Result);
        
        printf("WorldMemSize: %lu, SizeUsed: %lu\n\n", gameState->WorldArena.Size, gameState->WorldArena.Used);
        Assert(sizeof(Game_State) < gameMemory->MemorySize);
        
        // Render world
        const u32 totalChunks = world->ChunkCount*world->ChunkCount;
        for(u32 chunkIndex = 0; chunkIndex < totalChunks; chunkIndex++) {
            Chunk* chunk = world->Chunks + chunkIndex;
#if 1
            Render_Chunk(&openGL->RenderArena, world, chunk, &openGL->RenderData[chunkIndex]);
            LoadVertexBuffer(openGL, &openGL->RenderData[chunkIndex]);
            chunk->IsLoaded = true;
#else
            ChunkGen_Data* data = (ChunkGen_Data*)calloc(1, sizeof(*data));
            data->World = world;
            data->OpenGL = openGL;
            data->ChunkIndex = chunkIndex;
            data->NewPos = V2i(chunk->WorldPosition.x, chunk->WorldPosition.y);
            platformAPI->AddRenderWork(platformAPI->RenderQueue, ChunkGenWork, data);
            openGL->NewChunksToRender++;
#endif
        }
        openGL->RenderCount = totalChunks;
        
        gameMemory->IsInitialized = true;
    }
    
    //
    // Update the camera
    //m4x4 projection = Camera_CalculateProjectionMatrix(camera, (f32)openGL->WindowWidth, (f32)openGL->WindowHeight);
    glm::mat4 projection = Camera_CalculateProjectionMatrix(camera, (f32)openGL->WindowWidth, (f32)openGL->WindowHeight);
    glm::mat4 view = Camera_CalculateViewMatrix(camera);
    //m4x4 view = Camera_CalculateViewMatrix(camera);
    openGL->UniformMat4x42(openGL->ProjectionLoc, projection);
    openGL->UniformMat4x42(openGL->ViewLoc, view);
    openGL->UniformV3(openGL->SunPositionLoc, V3(1.0f, 355.0f, 1.0f));
    
    f32 sensitivity = 0.1f;
    f32 mx = input->MouseDeltaX;
    f32 my = input->MouseDeltaY;
    mx *= sensitivity;
    my *= sensitivity;
    
    camera->Orientation.x += my;
    camera->Orientation.y += mx;
    
    if(camera->Orientation.x > 89.0f) {
        camera->Orientation.x = 89.0f;
    }
    if(camera->Orientation.x < -89.0f) {
        camera->Orientation.x = -89.0f;
    }
    
    // TODO: Use dtime
    // Player Movement
    f32 playerSpeed = 0.05f;
    if(input->KeyPressed[InputKeyType_W]) {
        if(input->ShiftPressed) {
            playerSpeed += 0.10f;
        }
        camera->Position += camera->Forward * playerSpeed;
    } else if(input->KeyPressed[InputKeyType_S]) {
        camera->Position -= camera->Forward * playerSpeed;
    } else if(input->KeyPressed[InputKeyType_A]) {
        glm::vec3 left = glm::cross(camera->Forward, glm::vec3(0, 1, 0));
        camera->Position -= left * playerSpeed;
    } else if(input->KeyPressed[InputKeyType_D]) {
        glm::vec3 right = glm::cross(camera->Forward, glm::vec3(0, 1, 0));
        camera->Position += right * playerSpeed;
    }
    
#ifndef __EMSCRIPTEN__
    //
    // Load/Unload chunks
    Assert(world->ChunkCount%2 != 0);
    v2i playerPos = V2i((i32)(camera->Position.x/(f32)world->ChunkDim), (i32)(camera->Position.z/(f32)world->ChunkDim));
    if((i32)playerPos.x != world->LastPlayerChunkPos.x || (i32)playerPos.y != world->LastPlayerChunkPos.y)
    {
        enum Chunk_State {
            ChunkState_Unload,
            ChunkState_Loaded,
        };
        
        const i32 totalChunks = world->ChunkCount*world->ChunkCount;
        Chunk_State* chunkList = (Chunk_State*)calloc(totalChunks, sizeof(Chunk_State));
        std::vector<v2i> newChunks; // TODO: Remove this
        
        // Search for chunks out of bounds
        for(i32 chunkZ = playerPos.y - (i32)world->RenderDistance; 
            chunkZ <= playerPos.y + (i32)world->RenderDistance; 
            chunkZ++) {
            
            for(i32 chunkX = playerPos.x - (i32)world->RenderDistance; 
                chunkX <= playerPos.x + (i32)world->RenderDistance; 
                chunkX++) {
                //  new and old chunks
                i32 foundIndex = SearchForChunk(world, V2i(chunkX, chunkZ));
                
                if(foundIndex >= 0) {
                    chunkList[foundIndex] = ChunkState_Loaded;
                } else {
                    chunkList[foundIndex] = ChunkState_Unload;
                    newChunks.push_back(V2i(chunkX, chunkZ));
                }
            }
        }
        
        // Remove old chunks
        i32 removed = 0;
        for(i32 i = 0; i < totalChunks; i++) {
            Chunk* chunk = world->Chunks + i;
            
            if(chunkList[i] == ChunkState_Unload) {
                // Remove it from the GPU
                for(size_t index = 0; index < openGL->RenderCount; index++) {
                    
                    if(openGL->RenderData[index].ChunkPosition.x == chunk->WorldPosition.x &&
                       openGL->RenderData[index].ChunkPosition.y == chunk->WorldPosition.y) {
                        
                        openGL->RemoveBuffer(&openGL->RenderData[index].VAO, &openGL->RenderData[index].VBO);
                        openGL->RenderData[index].VertexCount = 0; // Only clear the size, so we can reuse the pointer
                        removed++;
                    }
                }
                memset(chunk->Blocks, 0, world->ChunkDim*world->ChunkDim*world->ChunkDimY*sizeof(Block));
            }
        }
        Assert(removed == (i32)newChunks.size());
        printf("-Removed: %d\n", removed);
        
        // Add new ones to the queue
        for(i32 i = 0; i < totalChunks; i++) {
            
            if(chunkList[i] == ChunkState_Unload) {
                ChunkGen_Data* data = (ChunkGen_Data*)calloc(1, sizeof(*data));
                data->World = world;
                data->OpenGL = openGL;
                data->ChunkIndex = i;
                data->NewPos = newChunks.back();
                platformAPI->AddRenderWork(platformAPI->RenderQueue, ChunkGenWork, data);
                newChunks.pop_back();
                openGL->NewChunksToRender++;
            }
        }
        
        // Update the render range
        world->LastPlayerChunkPos = playerPos;
        
        free(chunkList);
    }
    
    // Upload completed render chunks if any
    if(openGL->NewChunksToRender) {
        for(size_t renderIndex = 0; renderIndex < openGL->RenderCount; renderIndex++) {
            Chunk* chunk = world->Chunks + renderIndex;
            
            if(!chunk->IsLoaded && openGL->RenderData[renderIndex].VertexCount) {
                LoadVertexBuffer(openGL, &openGL->RenderData[renderIndex]);
                chunk->IsLoaded = true;
                openGL->NewChunksToRender--;
                //printf("load Chunk %lu\n", renderIndex);
            }
        }
        
        if(!openGL->NewChunksToRender) {
            Assert(platformAPI->RenderWorkCompleted(platformAPI->RenderQueue));
        }
    }
#endif
    
    // Draw triangles to screen
    openGL->DrawToScreen(openGL);
}