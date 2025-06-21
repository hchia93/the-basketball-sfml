#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <wchar.h>
#include <Windows.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

#define SFML sf
#define DELTA_TIME_STEP 0.02f
#define LOG(X) OutputDebugString(X)
#define LOG_CMD(X) std::cout<<X<<std::endl;
#define LOAD_CHECK(X, Path) if(!X) { std::string _s = "Cannot find file "; _s+=Path; _s+=". Load failed.\n"; LOG(_s.c_str());}
#define CORNFLOWER_BLUE sf::Color(100, 149, 237)


// Resources Path Defines.
#define RESOURCES_FONT_CHALK                "resource/rightchalk.ttf"
#define RESOURCES_FONT_PIXEL                "resource/04b03.ttf"
#define RESOURCES_TEXTURE_CHALKBOARD        "resource/chalkboard.jpg"
#define RESOURCES_TEXTURE_BASKETBALL        "resource/basketball.png"
#define RESOURCES_TEXTURE_BASKETBALLBOARD   "resource/basketballboard.png"
#define RESOURCES_TEXTURE_BACKGROUND        "resource/background.jpg"
#define RESOURCES_TEXTURE_WOOD              "resource/wood.jpg"
#define RESOURCES_TEXTURE_BOXALT            "resource/boxalt.png"
#define RESOURCES_AUDIO_TROLOLO             "resource/mr.ogg"

