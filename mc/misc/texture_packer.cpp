#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <malloc.h>
#include <dirent.h>
#include <sys/mman.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>

#include <stb_image.h>
#include <stb_image_write.h>

#define Assert(expression) if(!(expression)) { __builtin_trap(); }
typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t i32;
typedef float f32;
typedef i32 b32;

#include "mc_math.h"

struct UV_Info {
    u32 ItemCount;
    char Name[64];
    v2 UVS[4];
};

static void
WriteFile(char* filename, void* data, size_t size, b32 append = false) {
    // Get the file handle to write
    int fileHandle = -1;
    if(append)
        fileHandle = open(filename, O_WRONLY | O_APPEND);
    else
        fileHandle = open(filename, O_WRONLY | O_CREAT, S_IRUSR|S_IWUSR | S_IWGRP|S_IRGRP | S_IWOTH|S_IROTH);
    
    if(!fileHandle)
        return;
    
    // Write the file
    u32 bytesToWrite = size;
    u8* nextByteLocation = (u8*)data;
    while(bytesToWrite)
    {
        ssize_t bytesWritten = write(fileHandle, nextByteLocation, bytesToWrite);
        if(bytesWritten == -1)
        {
            close(fileHandle);
            return;
        }
        bytesToWrite -= bytesWritten;
        nextByteLocation += bytesWritten;
    }
    close(fileHandle);
}

static  void
PackTextures(char* imagesPath) {
    DIR* dir = opendir(imagesPath);
    
    if(!dir) return;
    
    dirent* entry;
    // Get the number of files
    size_t numOfFiles = 0;
    while((entry = readdir(dir))) {
        if(!strcmp(entry->d_name, "..") || !strcmp(entry->d_name, ".")) continue;
        
        const char* extension = entry->d_name + (strlen(entry->d_name)-4);
        if(!strcmp(extension, ".png")) {
            numOfFiles++;
        }
    }
    
    // Init the texture coordinates array
    UV_Info* texInfo = (UV_Info*)malloc(numOfFiles*sizeof(UV_Info));
    
    // Packing...
    int xCount = sqrtf(numOfFiles)+1;
    int outputWidth = xCount*32;
    //int outputWidth2 = (sqrt(numOfFiles)+1)*32; // NOTE: For some reason this gives a different result
    //printf("W: %d, W2: %d\n", outputWidth, outputWidth2);
    int totalSize = (outputWidth*outputWidth)*4;
    int currentX = 0, currentY = outputWidth-32;
    int yOffset = 0; //yOffset = 0;
    u8* dest = (u8*)mmap(0, totalSize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
    Assert(dest);
    
    size_t dirPathLen = strlen(imagesPath);
    b32 lastSlash = imagesPath[dirPathLen-1] == '/';
    
    size_t fileIndex = 0;
    rewinddir(dir); // Go back to the first file
    while((entry = readdir(dir))) {
        if(!strcmp(entry->d_name, "..") || !strcmp(entry->d_name, ".")) continue;
        
        const char* extension = entry->d_name + (strlen(entry->d_name)-4);
        if(strcmp(extension, ".png")) continue;
        
        size_t filenameLen = strlen(entry->d_name);
        if(filenameLen > PATH_MAX || dirPathLen > PATH_MAX) return;
        char fileFullPath[dirPathLen + filenameLen+1];
        snprintf(fileFullPath, dirPathLen+filenameLen+1, lastSlash ? "%s%s" : "%s/%s", imagesPath, entry->d_name);
        
        int width, height, channels;
        //stbi_set_flip_vertically_on_load(true); // Flip image
		// NOTE: You need to enforce 4 components per pixel or it will mess up the pixels
        u8* rawData = stbi_load(fileFullPath, &width, &height, &channels, 4); 
        Assert(rawData);
        Assert(height == 32 && width == 32);
        
		// Save the UV info
        UV_Info* item = &texInfo[fileIndex++];
        item->ItemCount = numOfFiles;
		Assert(filenameLen < sizeof(item->Name));
        memcpy(item->Name, entry->d_name, filenameLen);
		item->Name[filenameLen-4] = 0; // Remove the extension
		// Top Left
        item->UVS[0] = v2{ (f32)currentX / (f32)outputWidth, (f32)(yOffset + height) / (f32)outputWidth };
		// Top Right
        item->UVS[1] = v2{ (f32)(currentX + width) / (f32)outputWidth, (f32)(yOffset + height) / (f32)outputWidth };
		// Bottom Left
        item->UVS[2] = v2{ (f32)currentX / (f32)outputWidth, (f32)yOffset / (f32)outputWidth };
		// Bottom Right
        item->UVS[3] = v2{ (f32)(currentX + width) / (f32)outputWidth, (f32)yOffset / (f32)outputWidth };
        
        //printf("Index: %lu, File: %s\n", count++, entry->d_name); // @debuginfo
        u8* srcRow = rawData;
        u8* destRow = dest + currentY*outputWidth*4 + currentX*4;
        for(int y = 0; y < height; ++y) {
            
            u32* srcPixel = (u32*)srcRow;
            u32* destPixel = (u32*)destRow;
            for(int x = 0; x < width; ++x) {
                *destPixel++ = *srcPixel++;
            }
            srcRow += 4*width;
            destRow += 4*outputWidth;
        }
        free(rawData);
        currentX += width;
        if(currentX == outputWidth) {
            currentY -= height;
            yOffset += height;
            currentX = 0;
        }
    }
    closedir(dir);
    
    stbi_write_png("../assets/mc_atlas.png", outputWidth, outputWidth, 4, dest, outputWidth*4);
    munmap(dest, totalSize);
    
    // @temp
    // Save the texture info into a file
    WriteFile("../assets/uv_location.mc", texInfo, sizeof(UV_Info)*numOfFiles);  
    free(texInfo);
}

int main() {
	// Put all items images together in a PNG file
	PackTextures("../assets/images/block/");
	printf("DONE!\n");
    
	return 0;
}
