enum Cube_Face {
    FaceBack,
    FaceFront,
    FaceLeft,
    FaceRight,
    FaceBottom,
    FaceTop,
};

internal Render_Result 
Render_Chunk(World_Map* world, Chunk* chunk) {
    u32 chunkDimY = world->ChunkDimY;
    u32 chunkDimX = world->ChunkDim;
    u32 chunkDimZ = chunkDimX;
    Assert(chunkDimX <= 16 && chunkDimZ <= 16 && chunkDimY <= 255);
    
    enum {
        TopLeft,
        TopRight,
        BottomLeft,
        
        TopRight2,
        BottomRight,
        BottomLeft2,
    };
    
#if 0
    f32 uvs[] = {
        0.0f, 1.0f, // top left 
        1.0f, 1.0f, // top right
        0.0f, 0.0f, // bottom left
        
        1.0f, 1.0f, // top right
        1.0f, 0.0f, // bottom right
        0.0f, 0.0f, // bottom left
    };
#endif
    
#define Compress(x, y, z) (((z & 0xFF) << 16) | ((y & 0xFF) << 8) | (x & 0xFF))
#define UNIQUE_VERTICES_PER_CUBE 8
#define VERTEX_PER_FACE 6
#define FACES_PER_CUBE 6
#define VERTEX_PER_CUBE (VERTEX_PER_FACE*FACES_PER_CUBE)
    
    const size_t vertexCount = chunkDimX * chunkDimY * chunkDimZ * VERTEX_PER_CUBE;
    Vertex* vertexList = (Vertex*)calloc(vertexCount, sizeof(Vertex));
    
    Vertex* vertex = vertexList;
    for(i32 y = 0; y < (i32)chunkDimY; y++) {
        for(i32 z = 0; z < (i32)chunkDimZ; z++) {
            for(i32 x = 0; x < (i32)chunkDimX; x++) {
                
                Block_ID currentCubeType = Chunk_GetBlock(world, chunk, x, y, z).ID;
                if(currentCubeType == BlockID_None) continue; 
                Block_Format uv = GlobalBlockFormat[currentCubeType];
                
                if(x == (i32)chunkDimX-1 || z == (i32)chunkDimZ-1) {
                    uv = GlobalBlockFormat[BlockID_DiamondOre];
                }
                
                // NOTE: UV mapping saw it from the center of the cube, point of view remain the same.
                // Get the neighbor block so we can implement culling
                Block_ID leftBlock = Chunk_GetBlock(world, chunk, x-1, y, z).ID;
                Block_ID bottomBlock = Chunk_GetBlock(world, chunk, x, y-1, z).ID;
                Block_ID rightBlock = Chunk_GetBlock(world, chunk, x+1, y, z).ID;
                Block_ID topBlock = Chunk_GetBlock(world, chunk, x, y+1, z).ID;
                Block_ID backBlock = Chunk_GetBlock(world, chunk, x, y, z-1).ID;
                Block_ID frontBlock = Chunk_GetBlock(world, chunk, x, y, z+1).ID;
                
                enum {
                    UV_Top,
                    UV_Side,
                    UV_Bottom,
                };
                
                v3i cubeVertices[UNIQUE_VERTICES_PER_CUBE] = {
                    // Top: 
                    // Back left
                    V3i(x, y+1, z),       // 0
                    // Back right
                    V3i(x+1, y+1, z),      // 1
                    // Front left
                    V3i(x, y+1, z+1),      // 2
                    // Front right
                    V3i(x+1, y+1, z+1),    // 3
                    
                    // Bottom:
                    // Back left
                    V3i(x, y, z),      // 4
                    // Back right
                    V3i(x+1, y, z),    // 5
                    // Front left
                    V3i(x, y, z+1),    // 6
                    // Front right
                    V3i(x+1, y, z+1),  // 7
                };
                
                //
                // Back Face
                if(!backBlock) {
                    vertex[TopLeft].Position = Compress(cubeVertices[0].x, cubeVertices[0].y, cubeVertices[0].z);
                    vertex[TopLeft].NormalDirection = FaceBack;
                    vertex[TopRight].Position = Compress(cubeVertices[1].x, cubeVertices[1].y, cubeVertices[1].z);
                    vertex[TopRight].NormalDirection = FaceBack;
                    vertex[BottomLeft].Position = Compress(cubeVertices[4].x, cubeVertices[4].y, cubeVertices[4].z);
                    vertex[BottomLeft].NormalDirection = FaceBack;
                    //
                    vertex[TopRight2].Position = Compress(cubeVertices[1].x, cubeVertices[1].y, cubeVertices[1].z);
                    vertex[TopRight2].NormalDirection = FaceBack;
                    vertex[BottomRight].Position = Compress(cubeVertices[5].x, cubeVertices[5].y, cubeVertices[5].z);
                    vertex[BottomRight].NormalDirection = FaceBack;
                    vertex[BottomLeft2].Position = Compress(cubeVertices[4].x, cubeVertices[4].y, cubeVertices[4].z);
                    vertex[BottomLeft2].NormalDirection = FaceBack;
                    
                    
                    // Upper triangle
                    vertex[TopLeft].UV = uv.Faces[UV_Side].UVS[0]; // Top left
                    vertex[TopRight].UV = uv.Faces[UV_Side].UVS[1]; // Top right
                    vertex[BottomLeft].UV = uv.Faces[UV_Side].UVS[2]; // Bottom left
                    // Lower triangle
                    vertex[TopRight2].UV = uv.Faces[UV_Side].UVS[1]; // Top right
                    vertex[BottomRight].UV = uv.Faces[UV_Side].UVS[3]; // Bottom right
                    vertex[BottomLeft2].UV = uv.Faces[UV_Side].UVS[2]; // Bottom Left
                    
                    vertex += VERTEX_PER_FACE;
                }
                
                //
                // Front Face
                if(!frontBlock) {
                    vertex[TopLeft].Position = Compress(cubeVertices[3].x, cubeVertices[3].y, cubeVertices[3].z);
                    vertex[TopLeft].NormalDirection = FaceFront;
                    vertex[TopRight].Position = Compress(cubeVertices[2].x, cubeVertices[2].y, cubeVertices[2].z);
                    vertex[TopRight].NormalDirection = FaceFront;
                    vertex[BottomLeft].Position = Compress(cubeVertices[7].x, cubeVertices[7].y, cubeVertices[7].z);
                    vertex[BottomLeft].NormalDirection = FaceFront;
                    //
                    vertex[TopRight2].Position = Compress(cubeVertices[2].x, cubeVertices[2].y, cubeVertices[2].z);
                    vertex[TopRight2].NormalDirection = FaceFront;
                    vertex[BottomRight].Position = Compress(cubeVertices[6].x, cubeVertices[6].y, cubeVertices[6].z);
                    vertex[BottomRight].NormalDirection = FaceFront;
                    vertex[BottomLeft2].Position = Compress(cubeVertices[7].x, cubeVertices[7].y, cubeVertices[7].z);
                    vertex[BottomLeft2].NormalDirection = FaceFront;
                    
                    // Upper triangle
                    vertex[TopLeft].UV = uv.Faces[UV_Side].UVS[0]; // Top left
                    vertex[TopRight].UV = uv.Faces[UV_Side].UVS[1]; // Top right
                    vertex[BottomLeft].UV = uv.Faces[UV_Side].UVS[2]; // Bottom left
                    // Lower triangle
                    vertex[TopRight2].UV = uv.Faces[UV_Side].UVS[1]; // Top right
                    vertex[BottomRight].UV = uv.Faces[UV_Side].UVS[3]; // Bottom right
                    vertex[BottomLeft2].UV = uv.Faces[UV_Side].UVS[2]; // Bottom Left
                    
                    vertex += VERTEX_PER_FACE;
                }
                
                //
                // Left Face
                if(!leftBlock) {
                    vertex[TopLeft].Position = Compress(cubeVertices[2].x, cubeVertices[2].y, cubeVertices[2].z);
                    vertex[TopLeft].NormalDirection = FaceLeft;
                    vertex[TopRight].Position = Compress(cubeVertices[0].x, cubeVertices[0].y, cubeVertices[0].z);
                    vertex[TopRight].NormalDirection = FaceLeft;
                    vertex[BottomLeft].Position = Compress(cubeVertices[6].x, cubeVertices[6].y, cubeVertices[6].z);
                    vertex[BottomLeft].NormalDirection = FaceLeft;
                    //
                    vertex[TopRight2].Position = Compress(cubeVertices[0].x, cubeVertices[0].y, cubeVertices[0].z);
                    vertex[TopRight2].NormalDirection = FaceLeft;
                    vertex[BottomRight].Position = Compress(cubeVertices[4].x, cubeVertices[4].y, cubeVertices[4].z);
                    vertex[BottomRight].NormalDirection = FaceLeft;
                    vertex[BottomLeft2].Position = Compress(cubeVertices[6].x, cubeVertices[6].y, cubeVertices[6].z);
                    vertex[BottomLeft2].NormalDirection = FaceLeft;
                    
                    // Upper triangle
                    vertex[TopLeft].UV = uv.Faces[UV_Side].UVS[0]; // Top left
                    vertex[TopRight].UV = uv.Faces[UV_Side].UVS[1]; // Top right
                    vertex[BottomLeft].UV = uv.Faces[UV_Side].UVS[2]; // Bottom left
                    // Lower triangle
                    vertex[TopRight2].UV = uv.Faces[UV_Side].UVS[1]; // Top right
                    vertex[BottomRight].UV = uv.Faces[UV_Side].UVS[3]; // Bottom right
                    vertex[BottomLeft2].UV = uv.Faces[UV_Side].UVS[2]; // Bottom Left
                    
                    vertex += VERTEX_PER_FACE;
                }
                
                //
                // Right Face
                if(!rightBlock) {
                    vertex[TopLeft].Position = Compress(cubeVertices[1].x, cubeVertices[1].y, cubeVertices[1].z);
                    vertex[TopLeft].NormalDirection = FaceRight;
                    vertex[TopRight].Position = Compress(cubeVertices[3].x, cubeVertices[3].y, cubeVertices[3].z);
                    vertex[TopRight].NormalDirection = FaceRight;
                    vertex[BottomLeft].Position = Compress(cubeVertices[5].x, cubeVertices[5].y, cubeVertices[5].z);
                    vertex[BottomLeft].NormalDirection = FaceRight;
                    //
                    vertex[TopRight2].Position = Compress(cubeVertices[3].x, cubeVertices[3].y, cubeVertices[3].z);
                    vertex[TopRight2].NormalDirection = FaceRight;
                    vertex[BottomRight].Position = Compress(cubeVertices[7].x, cubeVertices[7].y, cubeVertices[7].z);
                    vertex[BottomRight].NormalDirection = FaceRight;
                    vertex[BottomLeft2].Position = Compress(cubeVertices[5].x, cubeVertices[5].y, cubeVertices[5].z);
                    vertex[BottomLeft2].NormalDirection = FaceRight;
                    
                    // Upper triangle
                    vertex[TopLeft].UV = uv.Faces[UV_Side].UVS[0]; // Top left
                    vertex[TopRight].UV = uv.Faces[UV_Side].UVS[1]; // Top right
                    vertex[BottomLeft].UV = uv.Faces[UV_Side].UVS[2]; // Bottom left
                    // Lower triangle
                    vertex[TopRight2].UV = uv.Faces[UV_Side].UVS[1]; // Top right
                    vertex[BottomRight].UV = uv.Faces[UV_Side].UVS[3]; // Bottom right
                    vertex[BottomLeft2].UV = uv.Faces[UV_Side].UVS[2]; // Bottom Left
                    
                    vertex += VERTEX_PER_FACE;
                }
                
                //
                // Bottom Face
                if(!bottomBlock) {
                    vertex[TopLeft].Position = Compress(cubeVertices[4].x, cubeVertices[4].y, cubeVertices[4].z);
                    vertex[TopLeft].NormalDirection = FaceBottom;
                    vertex[TopRight].Position = Compress(cubeVertices[5].x, cubeVertices[5].y, cubeVertices[5].z);
                    vertex[TopRight].NormalDirection = FaceBottom;
                    vertex[BottomLeft].Position = Compress(cubeVertices[6].x, cubeVertices[6].y, cubeVertices[6].z);
                    vertex[BottomLeft].NormalDirection = FaceBottom;
                    //
                    vertex[TopRight2].Position = Compress(cubeVertices[5].x, cubeVertices[5].y, cubeVertices[5].z);
                    vertex[TopRight2].NormalDirection = FaceBottom;
                    vertex[BottomRight].Position = Compress(cubeVertices[7].x, cubeVertices[7].y, cubeVertices[7].z);
                    vertex[BottomRight].NormalDirection = FaceBottom;
                    vertex[BottomLeft2].Position = Compress(cubeVertices[6].x, cubeVertices[6].y, cubeVertices[6].z);
                    vertex[BottomLeft2].NormalDirection = FaceBottom;
                    
                    // Upper triangle
                    vertex[TopLeft].UV = uv.Faces[UV_Bottom].UVS[0]; // Top left
                    vertex[TopRight].UV = uv.Faces[UV_Bottom].UVS[1]; // Top right
                    vertex[BottomLeft].UV = uv.Faces[UV_Bottom].UVS[2]; // Bottom left
                    // Lower triangle
                    vertex[TopRight2].UV = uv.Faces[UV_Bottom].UVS[1]; // Top right
                    vertex[BottomRight].UV = uv.Faces[UV_Bottom].UVS[3]; // Bottom right
                    vertex[BottomLeft2].UV = uv.Faces[UV_Bottom].UVS[2]; // Bottom Left
                    
                    vertex += VERTEX_PER_FACE;
                }
                
                //
                // Top Face
                if(!topBlock) {
                    vertex[TopLeft].Position = Compress(cubeVertices[1].x, cubeVertices[1].y, cubeVertices[1].z);
                    vertex[TopLeft].NormalDirection = FaceTop;
                    vertex[TopRight].Position = Compress(cubeVertices[0].x, cubeVertices[0].y, cubeVertices[0].z);
                    vertex[TopRight].NormalDirection = FaceTop;
                    vertex[BottomLeft].Position = Compress(cubeVertices[3].x, cubeVertices[3].y, cubeVertices[3].z);
                    vertex[BottomLeft].NormalDirection = FaceTop;
                    //
                    vertex[TopRight2].Position = Compress(cubeVertices[0].x, cubeVertices[0].y, cubeVertices[0].z);
                    vertex[TopRight2].NormalDirection = FaceTop;
                    vertex[BottomRight].Position = Compress(cubeVertices[2].x, cubeVertices[2].y, cubeVertices[2].z);
                    vertex[BottomRight].NormalDirection = FaceTop;
                    vertex[BottomLeft2].Position = Compress(cubeVertices[3].x, cubeVertices[3].y, cubeVertices[3].z);
                    vertex[BottomLeft2].NormalDirection = FaceTop;
                    
                    // Upper triangle
                    vertex[TopLeft].UV = uv.Faces[UV_Top].UVS[0]; // Top left
                    vertex[TopRight].UV = uv.Faces[UV_Top].UVS[1]; // Top right
                    vertex[BottomLeft].UV = uv.Faces[UV_Top].UVS[2]; // Bottom left
                    // Lower triangle
                    vertex[TopRight2].UV = uv.Faces[UV_Top].UVS[1]; // Top right
                    vertex[BottomRight].UV = uv.Faces[UV_Top].UVS[3]; // Bottom right
                    vertex[BottomLeft2].UV = uv.Faces[UV_Top].UVS[2]; // Bottom Left
                    
                    vertex += VERTEX_PER_FACE;
                }
            }
        }
    }
    
    //size_t sizeBytes = sizeUsed * sizeof(Vertex);
    
    size_t sizeUsed = vertex - vertexList;
    Render_Result data;
    data.VertexCount = sizeUsed;
    data.ChunkPosition.x = chunk->WorldPosition.x;
    data.ChunkPosition.y = chunk->WorldPosition.y;
    data.VertexData = vertexList;
    
    return data;
}