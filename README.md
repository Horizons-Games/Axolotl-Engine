# Axolotl Engine
This engine has been created by [Horizons Games](https://github.com/Horizons-Games/Axolotl-Engine) for the Master's Degree in Advanced Programming for AAA Videogames at UPC School.

# Installation
- Download the latest release
- Unzip the file
- Open AxolotlEngine.exe

# Features

**Docking**
- Move and dock your windows as you need.

**Scene management**
- Create a complex scene structure using hierarchical GameObjects.
- Add and remove GameObjects.
- Add and remove Components to GameObjects. 
- Edit Component Properties.
- Link GameObjects to other GameObjects.
- Reorder GameObjects in the scene hierarchy.

**File system**
- The whole scene can be serialised and unserialized (any GameObject and its components can be serialised into binary Files).
- Any scene can be saved or loaded using an ImGui menu options.
- A scene can be transformed into a JSON file. 
- Changing an asset in the inspector updated the corresponding resource.
- After modifying an asset from a local folder, the asset is updated automatically in the engine.
- Resource inspector where you can change the import and load options of a specific resource.
- There's a GAME mode that loads all the files using a compressed zip of all the required assets and custom binary files. To activate this, just define a GAME predefined compilation variable in Globals.h.

**Spatial Partitioning and Frustum Culling**
- Spatial Partitioning structure based on a QuadTree.
- The engine culls the objects of the Quadtree that are outside the camera frustum.
- The list of objects of the scene can be obtained by iterating the QuadTree.
- Draw Based on intersection Frustum/Quadtree <- When a quadtree is in the frustum draw its objects or call the intersection of their childs.
- Created Camera object that applies Frustum to the Scene.
- Draw Debug Utilities as Quadtree Bounding Box, object bounding boxes and Frustum.

**PBR Shading and Materials**
- Implementation of a Phong BRDF with Lambert Diffuse and Fresnel.
- Implementation of material maps (Diffuse, Specular and Shininess).
- Interface features: Load and delete material maps, change diffuse and specular's color and shininess and normal's strength. 

**Analytic lights and Normal map**
- Different types of lights (Ambient, Directional, Point and Spot lights).
- Lights can be added and removed and their parameters can be edited.
- Application of the Normal Map and the possibility to edit it.
- Implementation of the sky cubemap. 

**Area lights**
- Different types of Area lights (Sphere, tube).

**Breakable components**
- Create a breakable component and edit its impulsion power.

**Physics**
- Ability to create rigidbodies and edit its properties.
- Edit a Rigidbody collider and its properties (shape and transform).
- Draw Debug options for Rigidbody colliders.
- Add a proportional controller to a Rigidbody.

**Particle systems**
- Create a particle system and edit its properties (velocity, color, color gradient, spawnrate, texture, duration, lifespan, size, rotation, gravity, looping...).
- Add an emitter, select an emitter shape and customize properties (radius, angle).
- Add textures and spritesheet animations to a particle system.
- Initialize almost every property of the particle system using random values.

**Audio**
- Create and configure audio sources

**2D and 3D objects**
- Create 2D canvas (Empty, Text, Image, Button).
- Create 3D meshes (Cube, Sphere, Plane, Cylinder, Capsule, Character).

**Animations and state machines**
- Add an animation component to your meshes.
- Full state machine visual editor where you can create transitions and states with animations and control them on runtime with scripts. Accessible through the animation component.
- Draw Debug options for sketelal animation bones.

**Scripting**
- Create your own scripts from the Inspector.
- Support in the scripts a great amount of primitives, GameObjects, vectors, components and other scripts, and make them visible or not in the inspector.

**Build**
- Select the starting scene of your game, choose the build configuration and create a `.zip` file with all the assets and resources necessary to run the resulting executable file.

**Play scenes**
- Play or stop your scene at any moment.

# Controls
- Right click + A / D  to move camera around. (Left / Right)
- Right click + W / S to move camera around. (Front / Back)
- Right click + E / Q  to move camera around. (Top / Bottom)
- Right click to rotate the camera.
- Alt + left click to orbit the camera.
- Mouse wheel to zoom in/out.
- F to center the camera into the last loaded object.
- SHIFT + W / A / S / D to duplicate the movement speed.
- You can move, rotate and scale selected elements using ImGuizmo in the scene viewport.

# How to use
You can drag and drop different models and edit it's meshes and textures (loading from local) in the inspector window. 

**Menu Items**
- Windows (Enable and disable any window)
  - Scene
  - Configuration
  - Resources
  - Inspector
  - Hieriarchy
  - Editor Control
  - File Browser
  - Console
  
- Configuration
  - FPS (modify)
  - Renderer (Upload vertex and fragment shaders)
  - Hardware information
  - Window 
  - Camera editor
  - Frustum editor
 
 - Hierarchy
  - New Scene (create child, create camera)
    - Components (Create child, create camera, create 3D object, create light, create canvas, create audio source, move up, move down)
 
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

**Shortcuts**
- Q / W / E on scene window to change operation type of the gizmos.
- Ctrl + C to copy
- Ctrl + V to paste
- Ctrl + S to save
- Ctrl + Z to undo
- Ctrl + Y to redo

# Note

# License

MIT License

Copyright (c) 2022 Horizons Studio

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.





