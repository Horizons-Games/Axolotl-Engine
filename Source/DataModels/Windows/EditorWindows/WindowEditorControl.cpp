#include "WindowEditorControl.h"

#include "imgui.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "GameObject/GameObject.h"

#include "3DModels/Model.h"

#include <string>
#include <assert.h>


WindowEditorControl::WindowEditorControl() : EditorWindow("Editor Control")
{
    flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowEditorControl::~WindowEditorControl()
{
}

void WindowEditorControl::DrawWindowContents()
{
    DrawControlButtons();
}

void WindowEditorControl::DrawControlButtons()
{


    
}