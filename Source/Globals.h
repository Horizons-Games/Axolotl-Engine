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
#define MIN_QUADRANT_DIAGONAL 100.0f
 
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