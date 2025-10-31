# Sagittarius-A-OpenGL-simulation

Lightweight OpenGL simulation that visualizes null geodesics (light rays) around a Schwarzschild black hole. The project renders a simple sphere for the black hole and numerically integrates ray geodesics using an RK4 integrator. It uses GLFW + GLAD for OpenGL context and GLM for math.
![Untitled video 1- Made with Clipchamp](https://github.com/user-attachments/assets/2768e0ac-988f-4d34-9ccd-63150c0df4e0)

## Features
- Visual representation of a black hole (sphere mesh)
- Ray integration of null geodesics in the Schwarzschild metric with simple trails
- Small, dependency-contained demo (dependencies are in the `dependencies/` folder)

## Quick start (Windows / PowerShell)
Prerequisites:
- CMake (>= 3.12)
- A C++20-capable compiler toolchain (MSYS2/mingw-w64 or Visual Studio)

Build and run from the repository root in PowerShell:

```powershell
# Configure
cmake -S . -B build

# Build (parallel)
cmake --build build -- -j 2

# Run
.\build\Sagittarius_A.exe
```

Note: The project includes a pre-populated `dependencies/` folder with GLFW, GLAD headers and GLM headers. If you prefer system-installed dependencies, update `CMakeLists.txt` accordingly.

## Controls
- Right mouse drag: orbit/rotate camera around origin
- Scroll wheel: zoom in/out

## Project structure (important files)
- `CMakeLists.txt` — CMake build configuration
- `src/main.cpp` — program entry, constructs `App` and `BlackHole` and runs the app
- `src/controller/app.h`, `src/controller/app.cpp` — main application, GLFW setup, camera, main loop and shader setup
- `src/BlackHole.h`, `src/BlackHole.cpp` — simple UV-sphere mesh used to render the black hole
- `src/Ray.h`, `src/Ray.cpp` — ray struct, RK4 geodesic integrator, per-ray mesh and trails, Draw routine
- `src/view/shader.cpp` — helpers to load & compile GLSL files
- `src/shaders/vertex.txt`, `src/shaders/fragment.txt` — GLSL shader sources used by the program
- `dependencies/` — bundled third-party headers/libs (GLFW, GLAD, GLM, KHR)

## How it works
- The app creates many `Ray` objects with initial positions and velocities. Each frame the rays are advanced by `Ray::Step()` which performs an RK4 step of the Schwarzschild null geodesic ODEs.
- Ray trails are stored in an expanding vector (with a maximum length) and uploaded to a GL buffer each frame for rendering as line strips.
- The black hole is drawn as a simple indexed UV-sphere mesh.

## Tuning and development notes
- To change the number of rays, modify `App::InitializeRays(int)` in `src/controller/app.cpp`.
- Ray integration parameters (step size, max trail length, simulation scale) are in `src/Ray.*`.
- Shaders are plain GLSL in `src/shaders` and are loaded at runtime by `src/view/shader.cpp` — edit them to change lighting, coloring or to add effects.

## Adding features
- To add things like a warped ground grid, additional vertex data + a small vertex shader modification can be used. See `src/view/shader.cpp` for shader loading and `src/shaders/*.txt` for the shader code.



