#pragma once

#include "EngineLog.h"

#include <windows.h>

#define GLSL_VERSION "#version 440"

enum class update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

enum class TextureType 
{ 
	DIFFUSE, 
	NORMAL, 
	OCCLUSION,
	METALLIC,
	SPECULAR
};

// Application -------------
#define TITLE "Axolotl Engine"
#define VERSION "0.3.0"

// Configuration -----------
#define MAX_FRAMERATE 80 

// Configuration (Frustum)
#define QUADRANT_CAPACITY 30
#define MIN_CUBE_SIZE 100.0f
 
// Window ------------------
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define FULLSCREEN false
#define FULLSCREEN_DESKTOP false
#define BORDERLESS false
#define RESIZABLE true
#define VSYNC true
#define BRIGHTNESS 1.0f

// Paths -------------------

#define ASSETS_FOLDER "Assets"
#define LIB_FOLDER "Lib"
#define ASSETS_PATH "Assets/"
#define LIB_PATH "Lib/"
#define MODELS_PATH "Assets/Models/"
#define TEXTURES_PATH "Assets/Textures/"
#define MESHES_PATH "Assets/Meshes/"
#define MESHES_LIB_PATH "Lib/Meshes/"
#define MATERIAL_PATH "Assets/Materials/"
#define MATERIAL_LIB_PATH "Lib/Materials/"
#define ANIMATION_PATH "Assets/Animation/"
#define ANIMATION_LIB_PATH "Lib/Animation/"
#define SCENE_PATH "Assets/Scenes/"

// Resource extension ------
#define MODEL_EXTENSION ".fbx"
#define JPG_TEXTURE_EXTENSION ".jpg"
#define PNG_TEXTURE_EXTENSION ".png"
#define TIF_TEXTURE_EXTENSION ".tif"
#define DDS_TEXTURE_EXTENSION ".dds"
#define TGA_TEXTURE_EXTENSION ".tga"
#define HDR_TEXTURE_EXTENSION ".hdr"
#define SCENE_EXTENSION ".axolotl"
#define SKYBOX_EXTENSION ".sky"
#define CUBEMAP_EXTENSION ".cube"
#define MATERIAL_EXTENSION ".mat"
#define STATEMACHINE_EXTENSION ".state"
#define PARTICLESYSTEM_EXTENSION ".particle"
#define MESH_EXTENSION ".mesh"
#define ANIMATION_EXTENSION ".anim"
#define META_EXTENSION ".meta"
#define GENERAL_BINARY_EXTENSION ".bin"

// QuadTree standard -------
#define QUADTREE_INITIAL_SIZE 50
#define QUADTREE_INITIAL_ALTITUDE 10

