cmake_minimum_required(VERSION 3.16)

# Enable vcpkg manifest mode. This MUST be set before the project() command.
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(BasketballSFML VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(SFML 3.0 COMPONENTS Graphics Window System Audio REQUIRED)

# Fetch Box2D 2.4.1 directly from GitHub
include(FetchContent)
FetchContent_Declare(
  box2d
  URL https://github.com/erincatto/box2d/archive/refs/tags/v2.4.1.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(box2d)

# Set source files
set(SOURCES
    src/Private/main.cpp
    src/Private/Application.cpp
    src/Private/AssetLoader.cpp
    src/Private/Actor.cpp
    src/Private/ActorContactListener.cpp
    src/Private/GameState.cpp
    src/Private/TextProcessor.cpp
    src/Private/ActorRenderComponent.cpp
    src/Private/ActorPhysicComponent.cpp
)

set(HEADERS
    src/Public/Application.h
    src/Public/AssetLoader.h
    src/Public/Actor.h
    src/Public/ActorContactListener.h
    src/Public/Defines.h
    src/Public/GameState.h
    src/Public/Interface.h
    src/Public/TextProcessor.h
    src/Public/ActorRenderComponent.h
    src/Public/ActorPhysicComponent.h
)

# Create executable
add_executable(BasketballSFML
    src/Public/Application.h
    src/Public/AssetLoader.h
    src/Public/Actor.h
    src/Public/ActorContactListener.h
    src/Public/Defines.h
    src/Public/GameState.h
    src/Public/Interface.h
    src/Public/TextProcessor.h
    src/Public/ActorRenderComponent.h
    src/Public/ActorPhysicComponent.h
)

# Include directories
target_include_directories(BasketballSFML PRIVATE 
    src/Public
    ${SFML_INCLUDE_DIR}
    ${box2d_SOURCE_DIR}/include
)

target_include_directories(BasketballSFML PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Public"
    "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include"
    "${FETCHCONTENT_BASE_DIR}/box2d-src/include"
)

target_compile_definitions(BasketballSFML PRIVATE "PROJECT_ROOT_DIR=\"${CMAKE_SOURCE_DIR}\"")

# Link libraries
target_link_libraries(BasketballSFML PRIVATE 
    SFML::Graphics 
    SFML::Window 
    SFML::System 
    SFML::Audio
    box2d
)

# Copy resources to build directory
file(COPY resource DESTINATION ${CMAKE_BINARY_DIR})

# Set output directory
set_target_properties(BasketballSFML PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_command(TARGET BasketballSFML POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/resource"
    "$<TARGET_FILE_DIR:BasketballSFML>/Resources"
)

if(WIN32)
    target_sources(BasketballSFML PRIVATE
        src/Private/main.cpp
        src/Private/Application.cpp
        src/Private/AssetLoader.cpp
        src/Private/Actor.cpp
        src/Private/ActorContactListener.cpp
        src/Private/GameState.cpp
        src/Private/TextProcessor.cpp
        src/Private/ActorRenderComponent.cpp
        src/Private/ActorPhysicComponent.cpp
    )
    set_property(TARGET BasketballSFML PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:BasketballSFML>")
endif()