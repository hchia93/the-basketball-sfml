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
#define CORNFLOWER_BLUE SFML::Color(100, 149, 237)


// Resources Path Defines.
#define RESOURCES_FONT_CHALK				"Resources/RightChalk.ttf"
#define RESOURCES_FONT_PIXEL				"Resources/04b03.ttf"
#define RESOURCES_TEXTURE_CHALKBOARD		"Resources/chalkboard.jpg"
#define RESOURCES_TEXTURE_BASKETBALL		"Resources/basketBall.png"
#define RESOURCES_TEXTURE_BASKETBALLBOARD	"Resources/basketBallBoard.png"
#define RESOURCES_TEXTURE_BACKGROUND		"Resources/Background.jpg"
#define RESOURCES_TEXTURE_WOOD				"Resources/wood.jpg"
#define RESOURCES_TEXTURE_BOXALT			"Resources/boxAlt.png"
#define RESOURCES_AUDIO_TROLOLO				"Resources/Mr.ogg"

