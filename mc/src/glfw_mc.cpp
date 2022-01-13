#include "mc_platform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#if COMPILER_MSVC
#include <windows.h>
#else
#include <semaphore.h>
#include <pthread.h>
#endif

#include "opengl.cpp"

global Open_GL GlobalOpenGL;
global Game_Input GlobalInput;
global b32 GlobalMouseVisible;

struct Queue_Entry {
    work_callback* Callback;
    void* Data;
};
struct Work_Queue {
    volatile u32 NextEntry;
    volatile u32 EntryCount;
    volatile u32 CompletionCount;
#if COMPILER_MSVC
    HANDLE SemaphoreHandle;
#elif COMPILER_GCC
    sem_t SemaphoreHandle;
#endif
    
    Queue_Entry Entries[256];
};
struct Thread_Info {
    Work_Queue* Queue;
};

global Work_Queue GlobalRenderQueue;

#if COMPILER_MSVC
internal Read_Result
GLFW_ReadFile(char* filename) {
    Read_Result result = {};
    
    // Create the file handle
    HANDLE fileHandle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(fileHandle == INVALID_HANDLE_VALUE) {
        return result;
    }
    
    LARGE_INTEGER fileSize;
    if(!GetFileSizeEx(fileHandle, &fileSize)) {
        CloseHandle(fileHandle);
        return result;
    }
    
    u32 fileSize32 = (u32)fileSize.QuadPart;
    result.Data = VirtualAlloc(0, fileSize32, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    if(result.Data) {
        DWORD bytesRead;
        if(ReadFile(fileHandle, result.Data, fileSize32, &bytesRead, 0) && 
           (fileSize32 == bytesRead)) {
            // NOTE: File read successfully
            result.ContentSize = fileSize32;
        } else {
            VirtualFree(result.Data, 0, MEM_RELEASE);
            result.Data = 0;
        }
    }
    
    CloseHandle(fileHandle);
    
    return result;
}
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

internal Read_Result
GLFW_ReadFile(char* filename) {
    Read_Result result = {};
    
    int fileHandle = open(filename, O_RDONLY);
    if(fileHandle == -1) {
        return result;
    }
    
    struct stat fileStatus;
    if(fstat(fileHandle, &fileStatus) == -1)
    {
        close(fileHandle);
        return result;
    }
    result.ContentSize = fileStatus.st_size;
    
    result.Data = malloc(result.ContentSize);
    if(!result.Data)
    {
        close(fileHandle);
        result.ContentSize = 0;
        return result;
    }
    
    // Read the File
    u32 bytesToRead = result.ContentSize;
    u8* nextByteLocation = (u8*)result.Data;
    while(bytesToRead)
    {
        ssize_t bytesRead = read(fileHandle, nextByteLocation, bytesToRead);
        if(bytesRead == -1)
        {
            free(result.Data);
            result.Data = 0;
            result.ContentSize = 0;
            close(fileHandle);
            return result;
        }
        bytesToRead -= bytesRead;
        nextByteLocation += bytesRead;
    }
    close(fileHandle);
    
    return result;
}
#endif

internal void
GLFW_MousePositionChangeCallback(GLFWwindow* window, f64 x, f64 y) {
    (void)window;
    
    GlobalInput.MouseX = x;
    GlobalInput.MouseY = y;
    
    if(GlobalInput.FirstMouse) {
        GlobalInput.LastMouseX = x;
        GlobalInput.LastMouseY = y;
        GlobalInput.FirstMouse = false;
    }
    
    GlobalInput.MouseDeltaX = x - GlobalInput.LastMouseX;
    GlobalInput.MouseDeltaY = GlobalInput.LastMouseY - y;
    GlobalInput.LastMouseX = x;
    GlobalInput.LastMouseY = y;
}
internal void
GLFW_KeyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)window; (void)scancode; (void)mods;
    
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    }
    
    GlobalInput.ShiftPressed = mods == GLFW_MOD_SHIFT;
    b32 isPressed = action == GLFW_PRESS || action == GLFW_REPEAT;
    b32 isCtrlPressed = mods == GLFW_MOD_CONTROL;
    b32 isAltPressed = mods == GLFW_MOD_ALT;
    //b32 isMaximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
    // TODO: Check for windowed mode
    b32 isFullscreen = glfwGetWindowMonitor(window) != nullptr;
    
    switch(key) {
        case GLFW_KEY_W: GlobalInput.KeyPressed[InputKeyType_W] = isPressed; break;
        case GLFW_KEY_S: GlobalInput.KeyPressed[InputKeyType_S] = isPressed; break;
        case GLFW_KEY_A: GlobalInput.KeyPressed[InputKeyType_A] = isPressed; break;
        case GLFW_KEY_D: GlobalInput.KeyPressed[InputKeyType_D] = isPressed; break;
        case GLFW_KEY_Q: {
            if(isCtrlPressed) {
                glfwSetWindowShouldClose(window, GL_TRUE);
            }
            
            GlobalInput.KeyPressed[InputKeyType_Q] = isPressed; 
        } break;
        case GLFW_KEY_ENTER: {
            if(isAltPressed && isPressed) {
                
                if(!isFullscreen) {
                    //glfwMaximizeWindow(window);
                    
                    // Windowed mode 
                    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                } else {
                    //glfwRestoreWindow(window);
                    
                    glfwSetWindowMonitor(window, 0, 0, 0, 1280, 720, 0);
                }
            } 
        } break;
        case GLFW_KEY_BACKSLASH: {
            if(isAltPressed && isPressed) {
                GlobalMouseVisible = !GlobalMouseVisible;
                glfwSetInputMode(window, GLFW_CURSOR, GlobalMouseVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            }
        } break;
    }
}

internal void
GLFW_AddRenderWork(Work_Queue* queue, work_callback* callback, void* data) {
    if(queue->NextEntry == ArrayCount(queue->Entries)) return;
    //Assert(queue->EntryCount < ArrayCount(queue->Entries));
    Queue_Entry* entry = queue->Entries + queue->EntryCount;
    
    entry->Callback = callback;
    entry->Data = data;
    
    queue->EntryCount++;
#if COMPILER_MSVC
    ReleaseSemaphore(&queue->SemaphoreHandle, 1, 0);
#elif COMPILER_GCC
    sem_post(&queue->SemaphoreHandle);
#endif
}

internal b32
GLFW_DoNextWork(Work_Queue* queue) {
    b32 weShouldSleep = false;
    
    u32 originalNextEntry = queue->NextEntry;
    if(originalNextEntry < queue->EntryCount) {
#if COMPILER_MSVC
        u32 index = InterlockedCompareExchange((LONG volatile*)&queue->NextEntry, originalNextEntry+1, originalNextEntry);
#elif COMPILER_GCC
        u32 index = __sync_val_compare_and_swap(&queue->NextEntry, originalNextEntry, originalNextEntry+1);
#endif
        
        if(index == originalNextEntry) {
            Queue_Entry entry = queue->Entries[index];
            entry.Callback(queue, entry.Data);
#if COMPILER_MSVC
            InterlockedIncrement((LONG volatile*)&queue->CompletionCount);
#elif COMPILER_GCC
            __sync_add_and_fetch(&queue->CompletionCount, 1);
#endif
        }
    } else {
        weShouldSleep = true;
    }
    
    return weShouldSleep;
}

#if COMPILER_MSVC
DWORD WINAPI
#elif COMPILER_GCC
internal void*
#endif
GLFW_ThreadProc(void* param) {
    Work_Queue* queue = (Work_Queue*)param;
    
    for(;;) {
        
        if(GLFW_DoNextWork(queue)) {
#if COMPILER_MSVC
            WaitForSingleObjectEx(&queue->SemaphoreHandle, INFINITE, FALSE);
#elif COMPILER_GCC
            sem_wait(&queue->SemaphoreHandle);
            
#endif
        }
    }
    
    return 0;
}

internal void
GLFW_ClearRenderWork(Work_Queue* queue) {
    memset(queue->Entries, 0, sizeof(queue->Entries));
    queue->NextEntry = 0;
    queue->EntryCount = 0;
    queue->CompletionCount = 0;
}

internal b32
GLFW_RenderWorkCompleted(Work_Queue* queue) {
    b32 result = queue->CompletionCount == queue->EntryCount;
    
    return result;
}

internal void
GLFW_ResizeWindowCallback(GLFWwindow* window, int width, int height) {
    (void)window;
    GlobalOpenGL.WindowWidth = width;
    GlobalOpenGL.WindowHeight = height;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Center the window then show it
    //glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    GlobalOpenGL.WindowWidth = 1280;
    GlobalOpenGL.WindowHeight = 720;
    GLFWwindow* window = glfwCreateWindow(GlobalOpenGL.WindowWidth, GlobalOpenGL.WindowHeight, "GlobalOpenGL Template", 0, 0);
    if(!window) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, (mode->width-GlobalOpenGL.WindowWidth)/2, (mode->height-GlobalOpenGL.WindowHeight)/2);
    glfwShowWindow(window);
    
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Enable VSYNC
    glfwSwapInterval(1);
    // Callbacks
    glfwSetCursorPosCallback(window, GLFW_MousePositionChangeCallback);
    glfwSetKeyCallback(window, GLFW_KeyPressCallback);
    //glfwSetFramebufferSizeCallback();
    glfwSetWindowSizeCallback(window, GLFW_ResizeWindowCallback);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD.\n");
        return -1;
    }
    
    GlobalOpenGL.UniformU32 = OpenGL_UniformU32;
    GlobalOpenGL.UniformMat4x4 = OpenGL_UniformMat4x4;
    GlobalOpenGL.UniformV3 = OpenGL_UniformV3;
    GlobalOpenGL.UniformV2 = OpenGL_UniformV2;
    GlobalOpenGL.SendChunkBuffer = OpenGL_SendChunkBuffer;
    GlobalOpenGL.RemoveBuffer = OpenGL_RemoveBuffer;
    GlobalOpenGL.DrawToScreen = OpenGL_DrawToScreen;
    OpenGL_Init(&GlobalOpenGL);
    glViewport(0, 0, GlobalOpenGL.WindowWidth, GlobalOpenGL.WindowHeight);
    
#if COMPILER_MSVC
    HANDLE semaphoreHandle = CreateSemaphoreEx(0, 0, 1, 0, 0, SEMAPHORE_ALL_ACCESS);
#elif COMPILER_GCC
    sem_t semaphoreHandle;
    sem_init(&semaphoreHandle, 0, 1);
#endif
    GlobalRenderQueue.SemaphoreHandle = semaphoreHandle;
    
    Game_Memory gameMemory = {};
    gameMemory.MemorySize = Kilobytes(1);
    gameMemory.Memory = calloc(gameMemory.MemorySize, 1);
    gameMemory.PlatformAPI.ReadEntireFile = GLFW_ReadFile;
    gameMemory.PlatformAPI.AddRenderWork = GLFW_AddRenderWork;
    gameMemory.PlatformAPI.ClearRenderWork = GLFW_ClearRenderWork;
    gameMemory.PlatformAPI.RenderWorkCompleted = GLFW_RenderWorkCompleted;
    gameMemory.PlatformAPI.RenderQueue = &GlobalRenderQueue;
    
    GLuint textureSamplerID = Texture_Generate("../assets/mc_atlas.png");
    OpenGL_UniformI32(GlobalOpenGL.TextureLoc, textureSamplerID);
    glUseProgram(GlobalOpenGL.ProgramID);
    
#if COMPILER_MSVC
    DWORD threadID;
    HANDLE threadHandle = CreateThread(0, 0, GLFW_ThreadProc, &GlobalRenderQueue, 0, &threadID);
    CloseHandle(threadHandle);
#elif COMPILER_GCC
    pthread_t threadHandle;
    pthread_create(&threadHandle, 0, GLFW_ThreadProc, &GlobalRenderQueue);
    pthread_detach(threadHandle);
#endif
    
    int frames = 0;
    f64 lastTime = glfwGetTime();
    while(!glfwWindowShouldClose(window)) {
        glClearColor(153.0f/255.0f, 204.0f/255.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        Game_UpdateAndRender(&gameMemory, &GlobalInput, &GlobalOpenGL);
        
        glfwSwapBuffers(window);
        // Reset delta mouse
        GlobalInput.MouseDeltaX = 0;
        GlobalInput.MouseDeltaY = 0;
        glfwPollEvents();
        
        f64 endTime = glfwGetTime();
        f64 timeElapsed = endTime - lastTime;
        if(timeElapsed > 1.0 || frames == 0) {
            char fpsTitle[15];
            f64 fps = (f64)frames / (f64)timeElapsed;
            snprintf(fpsTitle, ArrayCount(fpsTitle), "FPS: %.2f", fps);
            glfwSetWindowTitle(window, fpsTitle);
            lastTime = endTime;
            frames = 0;
        }
        frames++;
    }
    
    glfwTerminate();
    return 0;
}