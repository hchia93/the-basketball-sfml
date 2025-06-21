# The Basketball

The Basketball was one of my university projects. The goal of this project is to show what kind of possible games to make with `Box2D` physics engine.


## Prerequisites

| Dependency | Version | Source |
|------------|---------|--------|
| SFML       | 2.5+    | https://www.sfml-dev.org/ |
| Box2D      | 2.4+    | https://github.com/erincatto/box2d |
| CMake      | 4.0.3+  | https://cmake.org/ |
| Visual Studio | 2019+ | https://visualstudio.microsoft.com/ |

> **Note:** CMake must be installed and available in your PATH (so you can run `cmake` from any command line). If you install CMake from https://cmake.org/download/, make sure to check the option to add CMake to your system PATH during installation.

## Project Setup
### Step 1: Install Dependencies
Run `SetupEnvironment.bat` to install `SFML` and `Box2D` dependencies. They should be installed at `C:\vcpkg\packages`

> **Note**: If you haven't installed `vcpkg` yet, please follow the [Installation Guide: vcpkg](https://github.com/hchia93/hchia93/blob/main/vcpkg-README.md) first.

### Step 2: Generate Visual Studio Solutions
Run `GenerateVisualStudioSolutions.bat` to generate Visual Studio solutions in the `generated-vs` directory.

### Step 3: Set the Startup Project in Visual Studio
After opening the generated solution in Visual Studio, **right-click on the project (e.g., `BasketballSFML`) in the Solution Explorer and select "Set as Startup Project"**. This ensures that pressing F5 or "Start" will launch the correct executable.

## Project Structure

```
the-basketball-sfml/
├── CMakeLists.txt                     #cmake-build
├── SetupEnvironment.bat               #instaling dependencies
├── GenerateVisualStudioSolutions.bat  #generate vs solution
├── README.md                          
├── src/                               #code
│   ├── Public/                        #headers files
│   └── Private/                       #impl files
├── resource/                          #assets
└── generated-vs/                      #generated content
```
