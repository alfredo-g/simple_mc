#include <SimplexNoise.h>

#include "mc.h"
#include "mc_camera.cpp"
#include "mc_world.cpp"
#include "mc_block.cpp"
#include "mc_render.cpp"

global std::vector<v2i> GlobalNewChunks;

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

#if 1
struct ChunkGen_Data {
    World_Map* World;
    std::vector<v2i>* NewChunks;
    i32 ReplaceChunkIndex;
};

internal WORK_QUEUE_CALLBACK(ChunkGenWork) {
    (void)queue;
    ChunkGen_Data* genData = (ChunkGen_Data*)data;
    i32 replaceIndex = genData->ReplaceChunkIndex;
    Chunk* chunk = genData->World->Chunks + replaceIndex;
    
    v2i last = genData->NewChunks->back();
    chunk->WorldPosition = v2i{last.x, last.y};
    Chunk_GenerateNew(chunk, genData->World->ChunkDim, genData->World->ChunkDimY, genData->World->WorldGenSeed);
    genData->NewChunks->pop_back();
    chunk->IsLoaded = false;
    free(data);
    //printf("+NewChunk: %d\n", replaceIndex);
}
#endif

inline void 
LoadVertexBuffer(Open_GL* openGL, Render_Result* render) {
    // Send the data to the GPU
    openGL->SendChunkBuffer(&render->VAO, &render->VBO, render->VertexCount*sizeof(Vertex), render->VertexData);
    // Free the vertex memory
    free(render->VertexData);
}

void Game_UpdateAndRender(Game_Memory* gameMemory, Game_Input* input, Open_GL* openGL) {
    
    Game_State* gameState = (Game_State*)gameMemory->Memory;
    Platform_API* platformAPI = &gameMemory->PlatformAPI;
    Camera_View* camera = &gameState->Camera;
    World_Map* world = &gameState->World;
    
    if(!gameMemory->IsInitialized) {
        
        Block_LoadLocationUV(platformAPI);
        
        //
        // World Init
        world->ChunkDim = 16;
        world->ChunkDimY = 255;
        world->ChunkCount = 11;
        world->RenderDistance = world->ChunkCount/2;
        //world->WorldGenSeed = 772;
        world->Chunks = (Chunk*)calloc((world->ChunkCount)*(world->ChunkCount), sizeof(Chunk));
        
        //
        // Camera Init
        const i32 x = world->ChunkCount/2;
        v2i center = v2i{x, x};
        camera->Position = glm::vec3(center.x*world->ChunkDim + (world->ChunkDim/2), 
                                     (f32)world->ChunkDimY, 
                                     center.y*world->ChunkDim + (world->ChunkDim/2));
        camera->FOV = 45.0f;
        camera->Orientation = glm::vec3(0.0f, -90.0f, 0.0f);
        
        // TODO: Take the player initial pos as reference
        world->RenderUpperBound = center + world->RenderDistance;
        world->RenderLowerBound = center - world->RenderDistance;
        
        World_GenerateMap(world);
        openGL->RenderData = (Render_Result*)calloc(world->ChunkCount*world->ChunkCount, sizeof(Render_Result));
        
        // TODO: Use memory arena
        printf("MemSize: %lu, StateSize: %lu\n\n", gameMemory->MemorySize, sizeof(Game_State));
        Assert(sizeof(Game_State) < gameMemory->MemorySize);
        
        // Render world
        const u32 totalChunks = world->ChunkCount*world->ChunkCount;
        for(u32 chunkIndex = 0; chunkIndex < totalChunks; chunkIndex++) {
            //Render_Result* render = openGL->RenderData + chunkIndex;
            Chunk* chunk = world->Chunks + chunkIndex;
            chunk->IsLoaded = true;
            
            openGL->RenderData[chunkIndex] = Render_Chunk(world, chunk);
            LoadVertexBuffer(openGL, &openGL->RenderData[chunkIndex]);
        }
        openGL->RenderCount = totalChunks;
        GlobalNewChunks.clear();
        
        gameMemory->IsInitialized = true;
    }
    
    //
    // Update the camera
    glm::mat4 projection = Camera_CalculateProjectionMatrix(camera, (f32)openGL->WindowWidth, (f32)openGL->WindowHeight);
    glm::mat4 view = Camera_CalculateViewMatrix(camera);
    openGL->UniformMat4x4(openGL->ProjectionLoc, projection);
    openGL->UniformMat4x4(openGL->ViewLoc, view);
    openGL->UniformV3(openGL->SunPositionLoc, v3{1.0f, 355.0f, 1.0f});
    
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
            playerSpeed = 0.10f;
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
    
    
#if 1
    //
    // Load/Unload chunks
    Assert(world->ChunkCount%2 != 0);
    //printf("Player: %f - %f\n", playerPos.x, playerPos.y);
    //printf("posGL: %f - %f\n\n", camera->Position.x, camera->Position.y);
    v2 playerPos = v2{camera->Position.x/(f32)world->ChunkDim, camera->Position.z/(f32)world->ChunkDim};
    if(!IsPlayerInRenderBounds(v2{playerPos.x, playerPos.y}, world)) {
        v2i upper = V2i(playerPos) + (i32)world->RenderDistance;
        v2i lower = V2i(playerPos) - (i32)world->RenderDistance;
        
        enum Chunk_State {
            ChunkState_Unload,
            ChunkState_Loaded,
        };
        
        const i32 totalChunks = world->ChunkCount*world->ChunkCount;
        Chunk_State* chunkList = (Chunk_State*)calloc(totalChunks, sizeof(Chunk_State));
        
        Assert(GlobalNewChunks.size() == 0);
        
        // Search for chunks out of bounds
        for(i32 chunkZ = lower.y; chunkZ <= upper.y; chunkZ++) {
            for(i32 chunkX = lower.x; chunkX <= upper.x; chunkX++) {
                //  new and old chunks
                i32 foundIndex = SearchForChunk(world, v2i{chunkX, chunkZ});
                
                if(foundIndex >= 0) {
                    chunkList[foundIndex] = ChunkState_Loaded;
                } else {
                    GlobalNewChunks.push_back(v2i{chunkX, chunkZ});
                }
            }
        }
        
        //printf("nchunks: %lu\n", ArrayCount(chunkList));
        //printf("newOnes: %lu\n", GlobalNewChunks.size());
        std::vector<ChunkGen_Data*> ChunkGenDataList;
        
        // Remove old chunks
        i32 removed = 0;
        for(i32 i = 0; i < totalChunks; i++) {
            Chunk* chunk = world->Chunks + i;
            
            if(chunkList[i] == ChunkState_Unload) {
                // Remove it from the GPU
                i32 renderIndex = -1;
                for(size_t index = 0; index < openGL->RenderCount; index++) {
                    
                    if(openGL->RenderData[index].ChunkPosition.x == chunk->WorldPosition.x &&
                       openGL->RenderData[index].ChunkPosition.y == chunk->WorldPosition.y) {
                        
                        openGL->RemoveBuffer(&openGL->RenderData[index].VAO, &openGL->RenderData[index].VBO);
                        openGL->RenderData[index] = {};
                        renderIndex = index;
                        removed++;
                    }
                }
                free(chunk->Blocks);
                chunk->Blocks = 0;
                Assert(renderIndex != -1);
                
                if(GlobalNewChunks.size() > 0) {
                    // Replace the old one with the new one
                    ChunkGen_Data* data = (ChunkGen_Data*)calloc(1, sizeof(*data));
                    data->World = world;
                    data->NewChunks = &GlobalNewChunks;
                    data->ReplaceChunkIndex = i;
                    platformAPI->AddRenderWork(platformAPI->RenderQueue, ChunkGenWork, data);
                    openGL->NewChunksToRender = true;
                }
            }
        }
        free(chunkList);
        //printf("unloadedCount: %d\n\n", removed);
        
        // Update the render range
        world->RenderUpperBound = upper;
        world->RenderLowerBound = lower;
    }
#endif
    
    // Upload completed render chunks if any
    if(openGL->NewChunksToRender && platformAPI->RenderWorkCompleted(platformAPI->RenderQueue)) {
        for(size_t renderIndex = 0; renderIndex < openGL->RenderCount; renderIndex++) {
            Chunk* chunk = world->Chunks + renderIndex;
            
            if(!chunk->IsLoaded) {
                openGL->RenderData[renderIndex] = Render_Chunk(world, chunk);
                
                LoadVertexBuffer(openGL, &openGL->RenderData[renderIndex]);
                chunk->IsLoaded = true;
            }
        }
        
        openGL->NewChunksToRender = false;
        // Clear the render queue
        platformAPI->ClearRenderWork(platformAPI->RenderQueue);
    }
    
    // Draw triangles to screen
    openGL->DrawToScreen(openGL);
}