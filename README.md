# Deferred Renderer

## Project Description
A deferred rendering engine designed to handle advanced graphics techniques with optimal performance. This project supports features such as multi-pass rendering, dynamic scene management, and advanced lighting effects.

## Features
- **Multi-pass Rendering:** Efficiently render complex scenes by separating rendering tasks into passes.
- **Dynamic Scene Management:** Manage and render scenes dynamically in real time.


# Project Setup Instructions

## Dependencies
This project requires **GLFW**, **OpenGL**, and **GLAD**.

### 1. GLAD (OpenGL Loader)
We need GLAD to load OpenGL 4.6 function pointers and extensions.

**Download Link**: [GLAD Generator Permalink](https://glad.dav1d.de/#language=c&specification=gl&api=gl%3D4.6&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&profile=core&extensions=GL_ARB_bindless_texture&extensions=GL_ARB_compute_shader&extensions=GL_ARB_direct_state_access&extensions=GL_ARB_indirect_parameters&extensions=GL_ARB_multi_draw_indirect&extensions=GL_ARB_shader_storage_buffer_object&extensions=GL_KHR_debug&loader=on)

**Included Extensions**:
- `GL_ARB_bindless_texture`
- `GL_ARB_compute_shader`
- `GL_ARB_direct_state_access`
- `GL_ARB_indirect_parameters`
- `GL_ARB_multi_draw_indirect`
- `GL_ARB_shader_storage_buffer_object`
- `GL_KHR_debug`

**Instructions**:
1. Click the link above.
2. Click **Generate**.
3. Download the zip file.
4. Copy `include/glad` and `include/KHR` folders to `/deffered_renderer/include/`.
5. Copy `src/glad.c` to `/deffered_renderer/src/`.

### 2. GLFW
1. Download the **Windows Pre-compiled Binaries** (32-bit or 64-bit depending on your MinGW/g++ version) from [glfw.org](https://www.glfw.org/download.html).
2. Extract the archive.
3. Copy the contents of the `include` folder (the `GLFW` folder) into `/deffered_renderer/include/`.
   - You should have `c:/dev/graphics/scene_tree/include/GLFW/glfw3.h`.
4. Copy the `libglfw3.a` file from the `lib-mingw-w64` (or similar) folder into `/deffered_renderer/lib/`.
   - **Important**: Do NOT copy `libglfw3dll.a` or `glfw3.dll`. We are using static linking.

### 2. Build
Run the build script:
```bash
python build.py
```

### 3. Run
Execute the generated application:
```bash
app.exe
```
