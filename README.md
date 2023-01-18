# Axolot Engine
This engine has been created by [Horizons Studio](https://github.com/Pre-SuperAwesomeEngine) for UPC's Master in Advanced Programming for AAA Videogames.
**FOTO MEGARECHULONA**

# Installation
- Download the latest release
- Unzip the file
- Open AxolotEngine.exe

# Features

**SCENE MANAGEMENT**
- Create a complex scene structure using hierarchical GameObjects.
- Add and remove GameObjects.
- Add and remove Components to GameObjects. 
- Edit Component Properties.
- Link GameObjects to other GameObjects.

**FILE SYSTEM**
- The whole scene can be serialised and unserialized (any GameObject and its components can be serialised into binary Files).
- Any scene can be save or load using an ImGui menu option.
- The scene can be change into a JSON scene and loaded. 
- Change an asset into the inspector the resource it's updated. 
- Modify the save asset that's in a local folder is upgraded. 
- There are a GAME mode, that load all the files using a zip file of custom files. To active this only define a GAME predefine compilation variable in Globals.h.

**Spatial Partitioning and Frustum Culling**
- Build the Spatial Partitioning structure Based on QuadTree.
- Have cull the objects/QuadTree that are inside the camera frustum.
- The list of objects of the scene can be obtained by iterating the QuadTree.
- Draw Based on intersection Frustum/Quadtree <- When A quadTree is in the frustum draw it objects or call the intersection of their childs .
- Created Camera Object that apply Frustum to the Scene
- Draw Debug Utilities as Quadtree Bounding Box, objects bounding box and Frustum

**PBR Shading and Materials**
- Implementation of a Phong BRDF with Lambert Diffuse and Fresnel.
- Implementation of material maps (Diffuse, Specular and Shininess).
- Interface features: Load and delete material maps, change diffuse and specular's color and shininess and normal's strength. 

**Analytic lights and Normal map**
- Different type of lights (Ambient, Directional, Point and Spot).
- Lights can be added and removed and their parameters can be edited.
- Application of the Normal Map and the possibility to edit it.
- Application of the sky CubeMap. 

# Controls
- Right click + “WASD” to move camera around.
- Right click to rotate the camera.
- Alt + left click to orbit the camera.
- Mouse wheel to zoom in/out.
- F to center the camera into the last loaded object.
- SHIFT + Movement to duplicate the movement speed.
- You can move, rotate and scale selected elements using ImGuizmo in the scene viewport.

# How to use
You can drag and drop different models and edit it's meshes and textures (loading from local) in the inspector window. 

**Menu Items**
- Windows (Able and disable any window)
  - Console
  - Scene
  - Configuration
  - Inspector
  - Hieriarchy
  - Editor Control
  - File Browser
  
- Configuration
  - FPS (modify)
  - Renderer (Upload vertex and fragment shaders)
  - Hardware information
  - Window 
  - Camera editor
  - Frustum editor
 
 - Hierarchy
  - New Scene (create child, create camera)
    - Components (Create child, create camera, move up, move down)
 
 - Inspector 
  - Load and save scene file
  - Enable lights
  - Transform, rotate and scale model. 
  - Bounding Box
  - Light Transform
  - **Right click**
    - Create mesh renderer.
    - Create and edit textures. 
    - Create light component. 

**Console panel**
- Prints all logged information and allows you to input commands.

**Editor Controller**
- Play/Pause controller

# Note

# License

MIT License

Copyright (c) 2022 Horizons Studio

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.





