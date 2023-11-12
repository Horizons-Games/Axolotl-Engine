#include "StdAfx.h"

#include "WindowDebug.h"

#include "Application.h"

#include "ImGui/imgui.h"

#include "Modules/ModuleRender.h"

#include "GL/glew.h"

bool WindowDebug::defaultEnabled = true;

WindowDebug::WindowDebug() : Window("DebugOptions")
{
}

WindowDebug::~WindowDebug()
{
}

void WindowDebug::Draw(bool& enabled)
{
	if (App->IsDebuggingGame())
	{
		// Activating the cursor like this for now
		SDL_ShowCursor(SDL_ENABLE);
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(
			ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.25f, io.DisplaySize.y * 0.25f), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.75f);
		if (ImGui::Begin(this->name.c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
		{
			ImGui::Text("Physics");

			/*
			if (ImGui::Checkbox("Wireframe Mode", &wireframeMode))
			{
				glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL);
			}
			*/

			ImGui::Checkbox("Draw Bounding Boxes", &drawBoundingBoxes);
			ImGui::Checkbox("Draw Physics", &drawPhysics);

			ImGui::Dummy(ImVec2(0, 5));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, 5));

			ImGui::Text("Graphics");

			ModuleRender* modRender = App->GetModule<ModuleRender>();
			bool drawShadows = modRender->IsShadowsEnabled();
			bool useVSM = modRender->IsVSMEnabled();
			bool drawSSAO = modRender->IsSsaoEnabled();
			bool drawBloom = modRender->IsBloomEnabled();
			bool useCSMDebug = modRender->IsCSMDebugEnabled();

			if (ImGui::BeginTable("##grahics", 2))
			{
				ImGui::TableNextColumn();
				if (ImGui::Checkbox("CSM Debug", &useCSMDebug))
				{
					modRender->ToggleCSMDebug();
				}
				ImGui::TableNextColumn();
				ImGui::Checkbox("Draw Area Lights", &drawAreaLight);
				ImGui::TableNextColumn();
				if (ImGui::Checkbox("Draw Shadows", &drawShadows))
				{
					modRender->ToggleShadows();
				};
				ImGui::TableNextColumn();
				ImGui::Checkbox("Draw Spot Lights", &drawSpotLight);
				ImGui::TableNextColumn();
				if (ImGui::Checkbox("Use VSM", &useVSM))
				{
					modRender->ToggleVSM();
				};
				ImGui::TableNextColumn();
				ImGui::Checkbox("Draw Directional Lights", &drawDirLight);
				ImGui::TableNextColumn();
				if (ImGui::Checkbox("Enable SSAO", &drawSSAO))
				{
					modRender->ToggleSSAO();
				};
				ImGui::TableNextColumn();
				ImGui::Checkbox("Draw Point Lights", &drawPointLight);
				ImGui::TableNextColumn();
				if (ImGui::Checkbox("Enable Bloom", &drawBloom))
				{
					modRender->SwitchBloomActivation();
				};
				ImGui::TableNextColumn();
				ImGui::TableNextColumn();
				if (ImGui::Button("Change Render Mode"))
				{
					modRender->ChangeRenderMode();
				}
				ImGui::TableNextColumn();
				ImGui::TableNextColumn();
				if (ImGui::Button("Change Tone Mapping"))
				{
					modRender->ChangeToneMapping();
				}
				ImGui::EndTable();
			}
			ImGui::End();
		}
	}
}
