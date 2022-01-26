GLuint Shader_CreateProgram(char* defines, char* shaderHeaderCode, char* vertexCode, char* fragmentCode) {
    GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
    GLchar* vertexShaderCode[] = {
        defines,
        shaderHeaderCode,
        vertexCode,
    };
    glShaderSource(vertexID, ArrayCount(vertexShaderCode), vertexShaderCode, 0);
    glCompileShader(vertexID);
    
    GLuint fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar* fragmentShaderCode[] = {
        defines,
        shaderHeaderCode,
        fragmentCode,
    };
    glShaderSource(fragmentID, ArrayCount(fragmentShaderCode), fragmentShaderCode, 0);
    glCompileShader(fragmentID);
    
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexID);
    glAttachShader(programID, fragmentID);
    glLinkProgram(programID);
    
    // Check for errors
    glValidateProgram(programID);
    GLint linked = false;
    glGetProgramiv(programID, GL_LINK_STATUS, &linked);
    if(!linked) {
        
        GLsizei Ignored;
        char VertexErrors[4096];
        char FragmentErrors[4096];
        char ProgramErrors[4096];
        glGetShaderInfoLog(vertexID, sizeof(VertexErrors), &Ignored, VertexErrors);
        glGetShaderInfoLog(fragmentID, sizeof(FragmentErrors), &Ignored, FragmentErrors);
        glGetProgramInfoLog(programID, sizeof(ProgramErrors), &Ignored, ProgramErrors);
        printf("Vertex Error:\n%s", VertexErrors);
        printf("Fragment Error:\n%s", FragmentErrors);
        printf("Program Error:\n%s", ProgramErrors);
        Assert(!"Shader validation failed");
    }
    
    return programID;
}

internal void 
OpenGL_UniformV3(i32 location, v3 value) {
    glUniform3f(location, value.x, value.y, value.z);
}
internal void 
OpenGL_UniformV2(i32 location, v2 value) {
    glUniform2f(location, value.x, value.y);
}
internal void 
OpenGL_UniformI32(i32 location, int value) {
    glUniform1i(location, value);
}
internal void 
OpenGL_UniformU32(i32 location, u32 value) {
    glUniform1ui(location, value);
}
#if 1
internal void 
OpenGL_UniformMat4x42(i32 location, glm::mat4& mat4) {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat4));
}
//#else
internal void 
OpenGL_UniformMat4x4(i32 location, m4x4 mat4) {
    glUniformMatrix4fv(location, 1, GL_FALSE, mat4.E[0]);
}
#endif

#if 0
internal void 
OpenGL_UniformV4(i32 location, glm::vec4& vec4) {
    glUniform4f(location, vec4.x, vec4.y, vec4.z, vec4.w);
}
internal void 
OpenGL_UniformF32(i32 location, f32 value) {
    glUniform1f(location, value);
}
internal void 
OpenGL_UniformMat3x3(i32 location, glm::mat3& mat3) {
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat3));
}
internal void 
OpenGL_UniformI32Array(i32 location, i32 length, i32* array) {
    glUniform1iv(location, length, array);
}
#endif

internal GLuint 
Texture_Generate(char* path) {
    stbi_set_flip_vertically_on_load(true); // Flip image
    int width, height, channels;
    u8* pixels = stbi_load(path, &width, &height, &channels, 4);
    Assert(pixels);
    
    GLuint textureSamplerID;
    glGenTextures(1, &textureSamplerID);
    glBindTexture(GL_TEXTURE_2D, textureSamplerID);
    
#if __EMSCRIPTEN__
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NONE);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    stbi_image_free(pixels);
    
    return textureSamplerID;
}

internal void
OpenGL_SendChunkBuffer(Open_GL* openGL, Render_Result* render) {
    const size_t vertexSizeInBytes = render->VertexCount*sizeof(Vertex);
    
    // VAO
    glGenVertexArrays(1, &render->VAO);
    glBindVertexArray(render->VAO);
    // VBO
    glGenBuffers(1, &render->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, render->VBO);
    //
    (void)openGL;
    glBufferData(GL_ARRAY_BUFFER, vertexSizeInBytes, render->VertexData, GL_STATIC_DRAW);
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, CompressData));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, UV));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

internal void
OpenGL_DrawToScreen(Open_GL* openGL) {
    glUseProgram(openGL->ProgramID);
    glViewport(0, 0, openGL->WindowWidth, openGL->WindowHeight);
    
    for(u32 index = 0; index < openGL->RenderCount; index++) {
        v2i chunkPos = openGL->RenderData[index].ChunkPosition;
        
        // Upload the chunk position to the GPU
        openGL->UniformV2(openGL->ChunkPosLoc, V2((f32)chunkPos.x, (f32)chunkPos.y));
        
        glBindVertexArray(openGL->RenderData[index].VAO);
        glDrawArrays(GL_TRIANGLES, 0, openGL->RenderData[index].VertexCount);
        //glDrawArrays(GL_POINTS, 0, openGL->RenderData[index].VertexCount);
        glBindVertexArray(0);
    }
}

internal void
OpenGL_RemoveBuffer(u32* vao, u32* vbo) {
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, vao);
}

internal void
OpenGL_Init(Open_GL* openGL) {
#ifdef __EMSCRIPTEN__
    b32 mobileBuild = true;
    char* openGLVersion = "300 es";
#else
    b32 mobileBuild = false;
    char* openGLVersion = "330 core";
#endif
    
    char optDefines[1024];
    snprintf(optDefines, sizeof(optDefines), "#version %s\n#define MOBILE_BUILD %d\n", openGLVersion, mobileBuild);
    
    //#version 330 core
    char* shaderHeaderCode = R"FOO(
        #define f32 float
        #define i32 int
        #define u32 uint
        #define v4 vec4
        #define v3 vec3
        #define v2 vec2
        )FOO";
    
    char* vertexCode = R"FOO(
#if MOBILE_BUILD
precision highp float;
#endif
layout (location = 0) in u32 aCompressData;
        layout (location = 1) in v2 aTexCoords;
        
uniform mat4 uProjection;
        uniform mat4 uView;
uniform v2 uChunkPos;

        smooth out v2 FragUV;
         flat out u32 CubeFace;
        smooth out v3 FragPosition;

void ExtractPosition(in u32 data, out v3 position) 
{
u32 x = data & u32(0xFF);
u32 y = (data >> 8) & u32(0xFF);
u32 z = (data >> 16) & u32(0xFF);
position = v3(f32(x), f32(y), f32(z));
}

void ExtractCubeFace(in u32 data, out u32 face)
{
face = (data >> 24) & u32(0xFF);
}
        
        void main()
        {
            v3 position;
ExtractPosition(aCompressData, position);
ExtractCubeFace(aCompressData, CubeFace);

// Converting to world coords
#define CHUNK_DIM 16.0
position.x += uChunkPos.x * CHUNK_DIM;
position.z += uChunkPos.y * CHUNK_DIM;

gl_Position = uProjection * uView * v4(position, 1.0);
        //gl_PointSize = 30.0;
            
        FragPosition = position;
FragUV = aTexCoords;
        }
        )FOO";
    char* fragmentCode = R"FOO(
#if MOBILE_BUILD
precision mediump float;
#endif
uniform v3 uSunPosition;
uniform sampler2D uTexture;

smooth in v2 FragUV;
 flat in u32 CubeFace;
smooth in v3 FragPosition;

out v4 ResultColor;

v3 lightColor = v3(1, 1, 1);

void FaceToNormal(in u32 face, out v3 normal) 
{
switch(face) 
{
case u32(0): normal = v3( 0,  0, -1); break;
case u32(1): normal = v3( 0,  0,  1); break;
case u32(2): normal = v3(-1,  0,  0); break;
case u32(3): normal = v3( 1,  0,  0); break;
case u32(4): normal = v3( 0, -1,  0); break;
case u32(5): normal = v3( 0,  1,  0); break;
}
}

void main()
{
// Calculate ambient light
 f32 ambientStrength = 0.4;
v3 ambient = ambientStrength * lightColor;

// Turn that into diffuse lighting
v3 lightDir = normalize(uSunPosition - FragPosition);
// Get the Normal
v3 normal;
FaceToNormal(CubeFace, normal);

f32 diff = max(dot(normal, lightDir), 0.0);
v3 diffuse = diff * lightColor;

v3 objectColor = texture(uTexture, FragUV).rgb;
v3 result = (ambient + diffuse) * objectColor;

ResultColor = v4(result, 1.0);
    //ResultColor = v4(1, 0, 0, 1);
}
)FOO";
    
    // TODO: Move the shader code to this file
    openGL->ProgramID = Shader_CreateProgram(optDefines, shaderHeaderCode, vertexCode, fragmentCode);
    glEnable(GL_DEPTH_TEST); // NOTE: This enable to render depth properly, also you need to clear the depth buffer
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
#if 0
    glEnable(GL_PROGRAM_POINT_SIZE);
#endif
    
    openGL->UniformU32 = OpenGL_UniformU32;
    openGL->UniformMat4x4 = OpenGL_UniformMat4x4;
    openGL->UniformMat4x42 = OpenGL_UniformMat4x42;
    openGL->UniformV3 = OpenGL_UniformV3;
    openGL->UniformV2 = OpenGL_UniformV2;
    openGL->SendChunkBuffer = OpenGL_SendChunkBuffer;
    openGL->RemoveBuffer = OpenGL_RemoveBuffer;
    openGL->DrawToScreen = OpenGL_DrawToScreen;
    
    openGL->TextureLoc = glGetUniformLocation(openGL->ProgramID, "uTexture");
    openGL->ViewLoc = glGetUniformLocation(openGL->ProgramID, "uView");
    openGL->ProjectionLoc = glGetUniformLocation(openGL->ProgramID, "uProjection");
    openGL->SunPositionLoc = glGetUniformLocation(openGL->ProgramID, "uSunPosition");
    openGL->ChunkPosLoc = glGetUniformLocation(openGL->ProgramID, "uChunkPos");
}