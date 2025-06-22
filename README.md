# The Basketball

The Basketball was one of my university projects. The goal of this project is to show what kind of possible games to make with `Box2D` physics engine.


## Prerequisites
To build and run this project, external dependencies are required. The `DependencyInstaller.bat` script will install them accordingly.

| Dependency | Version | Source |
|------------|---------|--------|
| SFML       | 2.6.1   | https://www.sfml-dev.org/          |
| Box2D      | 2.4.1   | https://github.com/erincatto/box2d |

## Project Structure

```
the-basketball-sfml/
├── CMakeLists.txt                     #cmake-build
├── vcpkg.json                         #vcpkg manifest (dependencies)
├── DependencyInstaller.bat            #installing dependencies
├── VisualStudioSolutionGenerator.bat  #generate vs solution
├── README.md                          
├── src/                               #code
│   ├── Public/                        #headers files
│   └── Private/                       #impl files
├── resource/                          #assets
└── generated-vs/                      #generated content
```

## Instruction
### Step 1: Installs Required Dependencies

Execute the following file to install relevant dependencies:

```cmd
DependencyInstaller.bat
```

### Step 2: Generates the Visual Studio Solution

Next, generate the Visual Studio Solution via the script. This creates the `generated-vs` directory.

```cmd
VisualStudioSolutionGenerator.bat
```

### Step 3: Build and Run

Open the generated solution located at:
- **`generated-vs/BasketBallSFML`**

Press **F5** to build and run the application.

> **⚠️ Important First-Time Setup**
> The first time you open the solution, you may need to manually set the startup project.
> 1. In the **Solution Explorer**, **right-click** on `generated-vs/BasketBallSFML`.
> 2. Select **"Set as StartUp Project"** from the menu.
>
> You only need to do this once.
