#include "mc_platform.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <emscripten/emscripten.h>
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include "opengl.cpp"

global Open_GL GlobalOpenGL;
global Game_Input GlobalInput;
global b32 GlobalMouseVisible;
global GLFWwindow* GlobalWindow;

#if 1
EM_JS(int, canvas_get_width, (), {
          return Module.canvas.width;
      });

EM_JS(int, canvas_get_height, (), {
          return Module.canvas.height;
      });
#endif

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

internal void 
GLFW_ErrorEventCallback(int error, const char* description) {
    fprintf(stderr, "Error %d: %s\n", error, description);
}

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
    (void)window; (void)scancode;
    
    GlobalInput.ShiftPressed = mods == GLFW_MOD_SHIFT;
    b32 isPressed = action == GLFW_PRESS || action == GLFW_REPEAT;
    b32 isCtrlPressed = mods == GLFW_MOD_CONTROL;
    b32 isAltPressed = mods == GLFW_MOD_ALT;
    
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
        case GLFW_KEY_BACKSLASH: {
            if(isAltPressed && isPressed) {
                GlobalMouseVisible = !GlobalMouseVisible;
                glfwSetInputMode(window, GLFW_CURSOR, GlobalMouseVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            }
        } break;
    }
}

extern "C" EMSCRIPTEN_KEEPALIVE void
API_ResizeWindow(int width, int height) {
    GlobalOpenGL.WindowWidth = width;
    GlobalOpenGL.WindowHeight = height;
}

internal void 
MainLoop(void* data) {
    Game_Memory* gameMemory = (Game_Memory*)data;
    
    glClearColor(153.0f/255.0f, 204.0f/255.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    Game_UpdateAndRender(gameMemory, &GlobalInput, &GlobalOpenGL);
    
    // Flip the double buffer
    glfwSwapBuffers(GlobalWindow);
    glfwPollEvents();
    // Reset delta mouse
    GlobalInput.MouseDeltaX = 0;
    GlobalInput.MouseDeltaY = 0;
}

int main() {
    // Start GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Error: GLFW Initialization failed.");
        emscripten_force_exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GlobalOpenGL.WindowWidth = canvas_get_width();
    GlobalOpenGL.WindowHeight = canvas_get_height();
    printf("Window W: %d, H: %d\n", GlobalOpenGL.WindowWidth, GlobalOpenGL.WindowHeight);
    // Create the display window
    GlobalWindow = glfwCreateWindow(GlobalOpenGL.WindowWidth, GlobalOpenGL.WindowHeight, "Simple MC", 0, 0);
    if (!GlobalWindow) {
        fprintf(stderr, "Error: GLFW Window Creation Failed");
        glfwTerminate();
        emscripten_force_exit(1);
    }
    glfwMakeContextCurrent(GlobalWindow); // Select the window as the drawing destination
    glfwSwapInterval(1); // Enable VSYNC
    glfwSetInputMode(GlobalWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Callbacks
    glfwSetErrorCallback(GLFW_ErrorEventCallback);
    glfwSetCursorPosCallback(GlobalWindow, GLFW_MousePositionChangeCallback);
    glfwSetKeyCallback(GlobalWindow, GLFW_KeyPressCallback);
    
    OpenGL_Init(&GlobalOpenGL);
    glViewport(0, 0, GlobalOpenGL.WindowWidth, GlobalOpenGL.WindowHeight);
    GLuint textureSamplerID = Texture_Generate("./assets/mc_atlas.png");
    // NOTE: glUseProgram() must go after OpenGL_UniformI32() otherwise the texture will not load (idk), 
    // ignore the firefox warning.
    OpenGL_UniformI32(GlobalOpenGL.TextureLoc, textureSamplerID); 
    glUseProgram(GlobalOpenGL.ProgramID);
    
    Game_Memory gameMemory = {};
    gameMemory.MemorySize = Megabytes(32);
    gameMemory.Memory = calloc(gameMemory.MemorySize, 1);
    gameMemory.PlatformAPI.ReadEntireFile = GLFW_ReadFile;
    gameMemory.PlatformAPI.AddRenderWork = 0; //GLFW_AddRenderWork;
    gameMemory.PlatformAPI.ClearRenderWork = 0; //GLFW_ClearRenderWork;
    gameMemory.PlatformAPI.RenderWorkCompleted = 0; //GLFW_RenderWorkCompleted;
    gameMemory.PlatformAPI.RenderQueue = 0; //&GlobalRenderQueue;
    
    //
    // Main Loop
    emscripten_set_main_loop_arg(MainLoop, (void*)&gameMemory, 0, true); //glfwWindowShouldClose(window);
    
    glfwDestroyWindow(GlobalWindow);
    glfwTerminate();
    return 0;
}