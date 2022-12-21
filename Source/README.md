# Chrysalis Engine :space_invader:

Game engine made in C++ by [Miquel Prieto Moliné](https://github.com/MacXxs) for the Masters' in AAA videogame development first assignment.</br>
You can check the repo [here](https://github.com/MacXxs/Engine) </br>

## Table of Contents 
- [How to use](https://github.com/MacXxs/Engine#how-to-use)               
</t> 1. [Camera Controls](https://github.com/MacXxs/Engine#camera-controls)  
</t> 2. [Laoding a Model](https://github.com/MacXxs/Engine#laoding-a-model) </br>
</t> 3. [Menu Layout](https://github.com/MacXxs/Engine#menu-layout) 
- [Additional Functionalities](https://github.com/MacXxs/Engine#additional-functionalities)

## How to use
### Camera Controls
- Use the "WASD" keys and use the mouse while right clicking to move in a FPS-fashion way and look around.
- Use the "QE" keys while right clicking to go up and down the scene.
- Holding down "LSHIFT" duplicates the movement.
- Use the mouse wheel to zoom in and out.
- Press "F" to focus the camera around a loaded model.
- Press "ALT" + left click to orbit around a loded model.

### Laoding a Model
To load a model simply drag and drop it on the scene. Any previous loaded model will be deleted.

### Menu Layout
- Window
  - Configuration
  - Camera
- Properties
- Console
- Help
  - Github page
  - About
- Quit

## Additional Functionalities
- The engine console shows every log done calling ENGINE_LOG, not just the assimp library log messages.
- The ability to modify the translation, rotation and scale of the loaded model through the transformation section on the properties window of the editor. The engine camera focuses and orbits around the OBB, so the camera adapts to any applied transformation.
