#pragma once

#include "EngineLog.h"

#include <windows.h>
#include <stdio.h>

#define GLSL_VERSION "#version 440"

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Application -------------
#define TITLE "Pre Super Awesome Engine"
#define VERSION "1.0.0"

// Configuration -----------
#define MAX_FRAMERATE 80 

// Configuration (Frustum)
#define QUADRANT_CAPACITY 10
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
#define MODELS_PATH "Assets/Models/"
#define TEXTURES_PATH "Assets/Textures/"
#define MESHES_PATH "Assets/Meshes/"
#define MESHES_LIB_PATH "Lib/Meshes/"
#define MATERIAL_LIB_PATH "Lib/Materials/"

// Resource extension ------
#define MODEL_EXTENSION ".fbx"
#define JPG_TEXTURE_EXTENSION ".jpg"
#define PNG_TEXTURE_EXTENSION ".png"
#define TIF_TEXTURE_EXTENSION ".tif"
#define DDS_TEXTURE_EXTENSION ".dds"
#define TGA_TEXTURE_EXTENSION ".tga"
#define SCENE_EXTENSION ".scene"
#define SKYBOX_EXTENSION ".sky"
#define MATERIAL_EXTENSION ".mat"
#define MESH_EXTENSION ".mesh"
#define META_EXTENSION ".meta"
#define GENERAL_BINARY_EXTENSION ".bin"
