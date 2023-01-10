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


    ImGuiStyle& style = ImGui::GetStyle();

    float size = ImGui::CalcTextSize("##Play").x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * 0.47;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);


    if (ImGui::ArrowButton("##Play", ImGuiDir_Right))
    {  
        if (!playButtonState)
        {
            scene->OnPlay();
            playButtonState = true;
        }
        else if (playButtonState)
        {
            scene->OnStop();
            playButtonState = false;
        }
        
    }
    ImGui::SameLine();
    if (ImGui::Button(" || "))
    {
        scene->OnPause();
        
    }
    ImGui::SameLine();
    

    
}