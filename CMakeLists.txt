cmake_minimum_required(VERSION 3.16)

# Enable vcpkg manifest mode. This MUST be set before the project() command.
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

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
    SFML::Graphics 
    SFML::Window 
    SFML::System 
    SFML::Audio
    box2d::box2d
)

# Copy resources to build directory
file(COPY resource DESTINATION ${CMAKE_BINARY_DIR})

# Set output directory
set_target_properties(BasketballSFML PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)