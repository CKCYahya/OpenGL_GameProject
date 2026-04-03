# GameProject

This project is a game/camera system developed with C++17 using OpenGL, GLFW, and ImGui. To keep the project repository lightweight, external dependencies (libraries) are not included on GitHub.

## Dependencies
To build the project, the following libraries must be downloaded and placed into the corresponding directories (`libs/` and `include/`) within the project:

* **GLFW** (Window and input management)
* **GLM** (Mathematics library)
* **ImGui** (Graphical user interface)
* **GLAD** (OpenGL function loader)
* **stb_image** (Image loader)
* **nlohmann_json** (JSON library for modern C++)

## Setup and Build Guide

### 1. Clone the Project
First, download the project to your computer and navigate into the directory:
```bash
git clone [https://github.com/CKCYahya/GameProject.git](https://github.com/CKCYahya/GameProject.git)
cd GameProject
```

### 2. Obtain the Libraries
For the project to compile correctly, you need to manually place the libraries into their respective folders:

**For the `libs/` Directory:**
Create a folder named `libs` in the root directory of the project and extract the following libraries into it:
* `libs/glfw/`
* `libs/glm/`
* `libs/imgui/`
* `libs/glad/`
* `libs/stb/` *(Placing just the `stb_image.h` file into this folder is sufficient)*

**For the `include/` Directory:**
* Create the `include/nlohmann/` folder. Download the library from the [nlohmann/json GitHub](https://github.com/nlohmann/json) page and place the necessary header files into this directory.

### 3. Build the Project (CMake)
Once the libraries are placed in the correct directories, you can build the project using standard CMake steps:

```bash
# Create the build directory and navigate into it
mkdir build
cd build

# Run CMake configuration
cmake ..

# Build the project
cmake --build .
```

### 4. Run the Project
After the build process is complete, launch the generated executable:

**For Windows:**
```bash
.\Debug\Camera.exe
# or
.\Camera.exe
```

**For Linux / macOS:**
```bash
./Camera
```