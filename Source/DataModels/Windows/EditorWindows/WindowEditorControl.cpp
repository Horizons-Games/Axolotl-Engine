#include "WindowEditorControl.h"

#include "imgui.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "GameObject/GameObject.h"

#include <string>
#include <assert.h>

static bool playButtonState = false;

WindowEditorControl::WindowEditorControl() : EditorWindow("Editor Control")
{
    flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowEditorControl::~WindowEditorControl()
{
}

void WindowEditorControl::DrawWindowContents()
{
    ImGuiStyle& style = ImGui::GetStyle();

    float size = ImGui::CalcTextSize("##Play").x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * 0.47f;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    if (ImGui::ArrowButton("##Play", ImGuiDir_Right))
    {
        (playButtonState) ? App->scene->OnStop() : App->scene->OnPlay();

        playButtonState = !playButtonState;
    }
    ImGui::SameLine();

    if (ImGui::Button("||"))
    {
        App->scene->OnPause();

    }
    ImGui::SameLine();
}