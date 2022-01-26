struct Block_Format {
    u32 ID;
    struct {
        v2 UVS[4]; // Top left, top right, bottom left, bottom right
    } Faces[3]; // Top, Side, Bottom
};

global Block_Format GlobalBlockFormat[BlockID_LAST];

internal void
Block_LoadLocationUV(Platform_API* platform) {
    struct UV_Info { /// NOTE: This struct is shared with texture_packer.cpp
        u32 ItemCount;
        char Name[64];
        v2 UVS[4];
    };
    
#ifdef __EMSCRIPTEN__
    Read_Result readed = platform->ReadEntireFile("./assets/uv_location.mc");
#else
    Read_Result readed = platform->ReadEntireFile("../assets/uv_location.mc");
#endif
    Assert(readed.ContentSize);
    Assert(readed.Data);
    
    UV_Info* info = (UV_Info*)readed.Data;
    
    // Map the texture name with the index for fast lookups
    std::unordered_map<std::string, u32> uvInfoMap;
    for(u32 index = 0; index < info->ItemCount; ++index) {
        std::string name = std::string(info[index].Name);
        uvInfoMap[name] = index;
    }
    
    //
    // Setup how each cube will look like
    Block_Format grass = {
        BlockID_Grass,
        {
            // Top
            {
#if 0
                info[uvInfoMap["grass_block_top"]].UVS[0],
                info[uvInfoMap["grass_block_top"]].UVS[1],
                info[uvInfoMap["grass_block_top"]].UVS[2],
                info[uvInfoMap["grass_block_top"]].UVS[3],
#else
                info[uvInfoMap["green_concrete_powder"]].UVS[0],
                info[uvInfoMap["green_concrete_powder"]].UVS[1],
                info[uvInfoMap["green_concrete_powder"]].UVS[2],
                info[uvInfoMap["green_concrete_powder"]].UVS[3],
#endif
            },
            // Side
            {
                info[uvInfoMap["grass_block_side"]].UVS[0],
                info[uvInfoMap["grass_block_side"]].UVS[1],
                info[uvInfoMap["grass_block_side"]].UVS[2],
                info[uvInfoMap["grass_block_side"]].UVS[3],
            },
            // Bottom
            {
                info[uvInfoMap["dirt"]].UVS[0],
                info[uvInfoMap["dirt"]].UVS[1],
                info[uvInfoMap["dirt"]].UVS[2],
                info[uvInfoMap["dirt"]].UVS[3],
            }
        }
    };
    GlobalBlockFormat[BlockID_Grass] = grass;
    
    Block_Format dirt = {
        BlockID_Dirt,
        {
            // Top
            {
                info[uvInfoMap["dirt"]].UVS[0],
                info[uvInfoMap["dirt"]].UVS[1],
                info[uvInfoMap["dirt"]].UVS[2],
                info[uvInfoMap["dirt"]].UVS[3],
            },
            // Side
            {
                info[uvInfoMap["dirt"]].UVS[0],
                info[uvInfoMap["dirt"]].UVS[1],
                info[uvInfoMap["dirt"]].UVS[2],
                info[uvInfoMap["dirt"]].UVS[3],
            },
            // Bottom
            {
                info[uvInfoMap["dirt"]].UVS[0],
                info[uvInfoMap["dirt"]].UVS[1],
                info[uvInfoMap["dirt"]].UVS[2],
                info[uvInfoMap["dirt"]].UVS[3],
            }
        }
    };
    GlobalBlockFormat[BlockID_Dirt] = dirt;
    
    Block_Format sand = {
        BlockID_Sand,
        {
            // Top
            {
                info[uvInfoMap["sand"]].UVS[0],
                info[uvInfoMap["sand"]].UVS[1],
                info[uvInfoMap["sand"]].UVS[2],
                info[uvInfoMap["sand"]].UVS[3],
            },
            // Side
            {
                info[uvInfoMap["sand"]].UVS[0],
                info[uvInfoMap["sand"]].UVS[1],
                info[uvInfoMap["sand"]].UVS[2],
                info[uvInfoMap["sand"]].UVS[3],
            },
            // Bottom
            {
                info[uvInfoMap["sand"]].UVS[0],
                info[uvInfoMap["sand"]].UVS[1],
                info[uvInfoMap["sand"]].UVS[2],
                info[uvInfoMap["sand"]].UVS[3],
            }
        }
    };
    GlobalBlockFormat[BlockID_Sand] = sand;
    
    Block_Format stone = {
        BlockID_Stone,
        {
            // Top
            {
                info[uvInfoMap["stone"]].UVS[0],
                info[uvInfoMap["stone"]].UVS[1],
                info[uvInfoMap["stone"]].UVS[2],
                info[uvInfoMap["stone"]].UVS[3],
            },
            // Side
            {
                info[uvInfoMap["stone"]].UVS[0],
                info[uvInfoMap["stone"]].UVS[1],
                info[uvInfoMap["stone"]].UVS[2],
                info[uvInfoMap["stone"]].UVS[3],
            },
            // Bottom
            {
                info[uvInfoMap["stone"]].UVS[0],
                info[uvInfoMap["stone"]].UVS[1],
                info[uvInfoMap["stone"]].UVS[2],
                info[uvInfoMap["stone"]].UVS[3],
            }
        }
    };
    GlobalBlockFormat[BlockID_Stone] = stone;
    
    Block_Format bedrock = {
        BlockID_Bedrock,
        {
            // Top
            {
                info[uvInfoMap["bedrock"]].UVS[0],
                info[uvInfoMap["bedrock"]].UVS[1],
                info[uvInfoMap["bedrock"]].UVS[2],
                info[uvInfoMap["bedrock"]].UVS[3],
            },
            // Side
            {
                info[uvInfoMap["bedrock"]].UVS[0],
                info[uvInfoMap["bedrock"]].UVS[1],
                info[uvInfoMap["bedrock"]].UVS[2],
                info[uvInfoMap["bedrock"]].UVS[3],
            },
            // Bottom
            {
                info[uvInfoMap["bedrock"]].UVS[0],
                info[uvInfoMap["bedrock"]].UVS[1],
                info[uvInfoMap["bedrock"]].UVS[2],
                info[uvInfoMap["bedrock"]].UVS[3],
            }
        }
    };
    GlobalBlockFormat[BlockID_Bedrock] = bedrock;
    
    Block_Format bookshelf = {
        BlockID_Bookshelf,
        {
            // Top
            {
                info[uvInfoMap["bookshelf"]].UVS[0],
                info[uvInfoMap["bookshelf"]].UVS[1],
                info[uvInfoMap["bookshelf"]].UVS[2],
                info[uvInfoMap["bookshelf"]].UVS[3],
            },
            // Side
            {
                info[uvInfoMap["bookshelf"]].UVS[0],
                info[uvInfoMap["bookshelf"]].UVS[1],
                info[uvInfoMap["bookshelf"]].UVS[2],
                info[uvInfoMap["bookshelf"]].UVS[3],
            },
            // Bottom
            {
                info[uvInfoMap["bookshelf"]].UVS[0],
                info[uvInfoMap["bookshelf"]].UVS[1],
                info[uvInfoMap["bookshelf"]].UVS[2],
                info[uvInfoMap["bookshelf"]].UVS[3],
            }
        }
    };
    GlobalBlockFormat[BlockID_Bookshelf] = bookshelf;
    
    Block_Format goldOre = {
        BlockID_GoldOre,
        {
            // Top
            {
                info[uvInfoMap["gold_ore"]].UVS[0],
                info[uvInfoMap["gold_ore"]].UVS[1],
                info[uvInfoMap["gold_ore"]].UVS[2],
                info[uvInfoMap["gold_ore"]].UVS[3],
            },
            // Side
            {
                info[uvInfoMap["gold_ore"]].UVS[0],
                info[uvInfoMap["gold_ore"]].UVS[1],
                info[uvInfoMap["gold_ore"]].UVS[2],
                info[uvInfoMap["gold_ore"]].UVS[3],
            },
            // Bottom
            {
                info[uvInfoMap["gold_ore"]].UVS[0],
                info[uvInfoMap["gold_ore"]].UVS[1],
                info[uvInfoMap["gold_ore"]].UVS[2],
                info[uvInfoMap["gold_ore"]].UVS[3],
            }
        }
    };
    GlobalBlockFormat[BlockID_GoldOre] = goldOre;
    
    Block_Format diamondOre = {
        BlockID_DiamondOre,
        {
            // Top
            {
                info[uvInfoMap["diamond_ore"]].UVS[0],
                info[uvInfoMap["diamond_ore"]].UVS[1],
                info[uvInfoMap["diamond_ore"]].UVS[2],
                info[uvInfoMap["diamond_ore"]].UVS[3],
            },
            // Side
            {
                info[uvInfoMap["diamond_ore"]].UVS[0],
                info[uvInfoMap["diamond_ore"]].UVS[1],
                info[uvInfoMap["diamond_ore"]].UVS[2],
                info[uvInfoMap["diamond_ore"]].UVS[3],
            },
            // Bottom
            {
                info[uvInfoMap["diamond_ore"]].UVS[0],
                info[uvInfoMap["diamond_ore"]].UVS[1],
                info[uvInfoMap["diamond_ore"]].UVS[2],
                info[uvInfoMap["diamond_ore"]].UVS[3],
            }
        }
    };
    GlobalBlockFormat[BlockID_DiamondOre] = diamondOre;
    
    Block_Format rail = {
        BlockID_Rail,
        {
            // Top
            {
                info[uvInfoMap["rail"]].UVS[0],
                info[uvInfoMap["rail"]].UVS[1],
                info[uvInfoMap["rail"]].UVS[2],
                info[uvInfoMap["rail"]].UVS[3],
            },
            // Side
            {
                info[uvInfoMap["rail"]].UVS[0],
                info[uvInfoMap["rail"]].UVS[1],
                info[uvInfoMap["rail"]].UVS[2],
                info[uvInfoMap["rail"]].UVS[3],
            },
            // Bottom
            {
                info[uvInfoMap["rail"]].UVS[0],
                info[uvInfoMap["rail"]].UVS[1],
                info[uvInfoMap["rail"]].UVS[2],
                info[uvInfoMap["rail"]].UVS[3],
            }
        }
    };
    GlobalBlockFormat[BlockID_Rail] = rail;
    
    free(info);
}