cmake_minimum_required(VERSION 3.16)
project(BasketballSFML VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(SFML 3.0 COMPONENTS Graphics Window System Audio REQUIRED)
find_package(Box2D REQUIRED)

# Set source files
set(SOURCES
    src/Private/main.cpp
    src/Private/Application.cpp
    src/Private/AssetLoader.cpp
    src/Private/b2Actor2D.cpp
    src/Private/b2Actor2DContactListener.cpp
    src/Private/GameState.cpp
    src/Private/TextRenderer.cpp
    src/Private/TickHandle.cpp
)

# Create executable
add_executable(BasketballSFML ${SOURCES})

# Include directories
target_include_directories(BasketballSFML PRIVATE src/Public)

# Link libraries
target_link_libraries(BasketballSFML PRIVATE 
    SFML-Graphics 
    SFML-Window 
    SFML-System 
    SFML-Audio
    box2d
)

# Copy resources to build directory
file(COPY resource DESTINATION ${CMAKE_BINARY_DIR})

# Set output directory
set_target_properties(BasketballSFML PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Windows-specific settings
if(WIN32)
    # Copy SFML DLLs to output directory
    add_custom_command(TARGET BasketballSFML POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-graphics>
        $<TARGET_FILE:sfml-window>
        $<TARGET_FILE:sfml-system>
        $<TARGET_FILE:sfml-audio>
        $<TARGET_FILE_DIR:BasketballSFML>
    )
endif() 