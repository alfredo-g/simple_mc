#ifndef MC_PLATFORM_H
#define MC_PLATFORM_H

#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#elif __GNUC__
#undef COMPILER_GCC
#define COMPILER_GCC 1
#endif

#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <time.h>
#include <unordered_map>

// TODO: Remove glm
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef i32 b32;
typedef float f32;
typedef double f64;

#if COMPILER_MSVC
#define Assert(expression) if(!(expression)) { *((int*)0) = 0; }
#else
#define Assert(expression) if(!(expression)) { __builtin_trap(); }
#endif

#define ArrayCount(array) (sizeof(array)/sizeof((array)[0]))
#define MAX(a, b) (a > b ? a : b)
#define internal static
#define global static

#define Kilobytes(value) ((value)*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)
#define Terabytes(value) (Gigabytes(value)*1024LL)

#include "mc_math.h"

struct Read_Result {
    u32 ContentSize;
    void* Data;
};

struct Work_Queue;
#define WORK_QUEUE_CALLBACK(name) void name(Work_Queue* queue, void* data)
typedef WORK_QUEUE_CALLBACK(work_callback);

struct Platform_API {
    Work_Queue* RenderQueue;
    
    Read_Result (*ReadEntireFile)(char* filename);
    void (*AddRenderWork)(Work_Queue* queue, work_callback* callback, void* data);
    void (*ClearRenderWork)(Work_Queue* queue);
    b32 (*RenderWorkCompleted)(Work_Queue* queue);
};

struct Vertex {
    union {
        u32 CompressData;
        struct {
            u32 Position : 24; // X : 8 bits, Z : 8 bits, Y : 8 bits
            u32 NormalDirection : 8;
        };
    };
    v2 UV;
};

struct Render_Result {
    u32 VAO;
    u32 VBO;
    v2i ChunkPosition;
    u32 VertexCount;
    
    Vertex* VertexData;
};

struct Open_GL {
    int WindowWidth, WindowHeight;
    
    // Shader program
    u32 ProgramID;
    Render_Result* RenderData;
    size_t RenderCount;
    b32 NewChunksToRender;
    // Uniform locations
    i32 TextureLoc;
    i32 ViewLoc;
    i32 ProjectionLoc;
    i32 SunPositionLoc;
    i32 ChunkPosLoc;
    //
    void (*RemoveBuffer)(u32* vao, u32* vbo);
    void (*DrawToScreen)(Open_GL* openGL);
    void (*SendChunkBuffer)(u32* vao, u32* vbo, size_t vertexSizeBytes, void* vertexData);
    void (*UniformU32)(i32 nameLoc, u32 value);
    void (*UniformV3)(i32 nameLoc, v3 value);
    void (*UniformV2)(i32 nameLoc, v2 value);
    void (*UniformMat4x4)(i32 nameLoc, glm::mat4& mat4);
};

enum Game_Input_Key_Type {
    InputKeyType_W,
    InputKeyType_S,
    InputKeyType_A,
    InputKeyType_D,
    InputKeyType_Q,
    InputKeyType_LAST,
};

struct Game_Input {
    f32 MouseX, MouseY;
    f32 MouseDeltaX, MouseDeltaY;
    b32 KeyPressed[InputKeyType_LAST];
    b32 ShiftPressed;
    // TODO: dtPerFrame;
    // 
    f32 LastMouseX, LastMouseY;
    b32 FirstMouse = true; // Init flag(?)
};

struct Game_Memory {
    b32 IsInitialized;
    
    void* Memory;
    u64 MemorySize;
    Platform_API PlatformAPI;
};

extern "C" void Game_UpdateAndRender(Game_Memory* gameMemory, Game_Input* input, Open_GL* openGL);

#endif //MC_PLATFORM_H
