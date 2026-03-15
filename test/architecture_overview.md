# Voxel Engine Architecture Overview

## 🏗️ 1. Project Structure
The engine is split cleanly into distinct modules to separate concerns and allow for future modularity (e.g. extending physics, multi-threading terrain, or dropping in an ECS over the core data):
- **`src/main.cpp`**: Application shell, constructs modules and drives the game loop.
- **`src/renderer/`**: Handles all GPU, Vulkan, and Graphics-related logic. Completely blind to game logic. Let's the `World` dictate what meshes it needs to render.
- **`src/world/`**: CPU-side voxel logic, proc-gen chunks, coordinate transformations.
- **`src/input/`**: Abstraction over GLFW for mouse/keyboard tracking.
- **`src/util/`**: Helpers (e.g., IO file reading for shaders).
- **`shaders/`**: GLSL code.

## 🖼️ 2. Vulkan 1.3 Target & Dynamic Rendering
The engine is explicitly built on **Vulkan 1.3** and avoids legacy bloat:
- **No Render Passes**: Employs `VK_KHR_dynamic_rendering` via `vkCmdBeginRendering()` to simplify pipelines and allow for flexible rendering sequences (shadows, distinct passes, transparency).
- **No Framebuffers**: Render targets are passed directly to the dynamic rendering calls.
- **Explicit Synchronization**: Managed via Fences, Semaphores, and precise Pipeline Barriers.
- **Descriptor Sets**: Uses UBOs for environment lighting and combined image samplers for the procedural Texture Atlas.

## 🌍 3. World & Chunk Geometry (Optimized)
- **Local vs World Coordinate Systems**: `World.cpp` intelligently translates global vectors down to `Chunk` positions and `Block` offsets, abstracting away chunk boundaries.
- **Chunk Geometry Generation**: Built dynamically per chunk (using `isDirty` state tracking).
- **Face Culling (Occlusion)**: `ChunkMesh::buildMeshData()` does **not** generate internal faces. It searches neighbors (even across immediate chunk boundaries dynamically via the context) and only outputs vertices exposed to air/transparent blocks.
- **Lighting per Vertex**: Approximates Fake AO / Static shadows based on whether faces point UP, DOWN, or SIDE. This is scaled in the Fragment Shader.

## ⚙️ 4. Low-end Hardware Optimizations 
- Minimal State changes: Entire world uses ONE material bind (`vkCmdBindDescriptorSets`), ONE `VkPipeline`, and uses `Push Constants` to send the fast-changing MVP Matrices per frame to avoid updating uniform buffers entirely for position matrices!
- **Greedy Meshing Target**: Architecture groups vertex buffers statically inside `<renderer/ChunkMesh>`.
- Chunks run logic CPU bound. Memory mapping utilizes `VK_MEMORY_PROPERTY_HOST_COHERENT_BIT` dynamically.

## 🔑 5. Compiling
1. Install Vulkan SDK.
2. Run CMake:
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```
3. CMake will compile GLSL automatically to `.spv` provided `glslc` is in your PATH / SDK Environment variables.
