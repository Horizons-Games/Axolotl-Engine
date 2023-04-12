#pragma once
// RCC++ headers
#include "RuntimeInclude.h"
RUNTIME_MODIFIABLE_INCLUDE; //recompile runtime files when this changes

class GameObject;


struct SystemTable
{
    GameObject* owner = 0;
};