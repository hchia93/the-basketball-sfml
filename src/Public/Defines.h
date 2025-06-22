#pragma once
#include <string>
#include <iostream>
#include <Windows.h>
#include <SFML/Graphics.hpp>

// Physics constants
static const float DELTA_TIME_STEP = 0.02f; // 50 FPS fixed timestep

// Window and rendering constants
static const sf::Color CORNFLOWER_BLUE = sf::Color(100, 149, 237, 255);

// Resources Path Defines.
#define RESOURCES_FONT_CHALK				"Resources/rightchalk.ttf"
#define RESOURCES_FONT_PIXEL				"Resources/04b03.ttf"
#define RESOURCES_TEXTURE_CHALKBOARD		"Resources/chalkboard.jpg"
#define RESOURCES_TEXTURE_BASKETBALL		"Resources/basketball.png"
#define RESOURCES_TEXTURE_BASKETBALLBOARD	"Resources/basketballboard.png"
#define RESOURCES_TEXTURE_BACKGROUND		"Resources/background.jpg"
#define RESOURCES_TEXTURE_WOOD				"Resources/wood.jpg"
#define RESOURCES_TEXTURE_BOXALT			"Resources/boxalt.png"
#define RESOURCES_AUDIO_TROLOLO				"Resources/Mr.ogg"

// Logging macros
#define LOG(X) OutputDebugString(X)
#define LOG_CMD(X) std::cout<<X<<std::endl;
#define LOAD_CHECK(X, Path) if(!X) { std::string _s = "Cannot find file "; _s+=Path; _s+=". Load failed.\n"; LOG(_s.c_str());}

// Resource identifiers
namespace RESOURCES
{
    // Textures
    static const char* TEXTURE_BACKGROUND = "background.jpg";
    static const char* TEXTURE_CHALKBOARD = "chalkboard.jpg";
    static const char* TEXTURE_BOXALT = "boxalt.png";
    static const char* TEXTURE_BASKETBALL = "basketball.png";
    static const char* TEXTURE_BASKETBALLBOARD = "basketballboard.png";
    static const char* TEXTURE_WOOD = "wood.jpg";

    // Fonts
    static const char* FONT_CHALK = "rightchalk.ttf";
    static const char* FONT_PIXEL = "04b03.ttf";

    // Audio
    static const char* AUDIO_TROLOLO = "mr.ogg";
}

// Actor shape types
enum class EActorShapeType
{
    Rectangle,
    Circle,
    Convex,
};

// Box2D shape types
enum class Eb2ShapeType
{
    Polygon,
    Circle,
    Edge,
    Chain,
};

