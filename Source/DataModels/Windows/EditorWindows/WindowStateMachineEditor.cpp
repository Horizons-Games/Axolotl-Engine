#include "WindowStateMachineEditor.h"

#include "imgui.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceStateMachine.h"
#include "DataModels/Resources/EditorResource/EditorResourceInterface.h"
#include "ModuleEditor.h"
#include "ImporterWindows/WindowResourceInput.h"

#include "Geometry/LineSegment2D.h"

#include "Auxiliar/Reflection/Field.h"
#include "Auxiliar/Reflection/TypeToEnum.h"

WindowStateMachineEditor::WindowStateMachineEditor() : EditorWindow("StateMachineEditor"),
stateIdSelected(-1), creatingTransition(false), openContextMenu(false), sizeState(200,50), inputResource(std::make_unique<WindowResourceInput>())
{
	stateMachine = std::make_shared<ResourceStateMachine>(0, "", "", "");
}

void WindowStateMachineEditor::DrawWindowContents()
{
	std::shared_ptr<ResourceStateMachine> stateAsShared = stateMachine;
	ImGui::BeginChild("Side_lists", ImVec2(200, 0));
	ImGui::Text("Parameters");
	/*if (ImGui::Button("NEW FIELD")) 
	{

		Field<float> field(
			"NAME", 
			[this] { return this->Get##Name(); },
			[this](const Type& value) { this->Set##Name(value)
		);

		TypeToEnum<char>::value;

	}*/
	ImGui::Separator();
	ImGui::Text("");
	if (stateAsShared && stateIdSelected < stateAsShared->GetNumStates() && stateIdSelected >= 0)
	{
		State* state = stateAsShared->GetStates()[stateIdSelected];
		ImGui::Text("State");
		ImGui::Separator();
		std::string name = state->name;
		name.resize(24);
		if (ImGui::InputText("Name:", &name[0], 24, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			stateAsShared->SetStateName(stateIdSelected, name);
		}

		if(state->resource == nullptr)
		{
			inputResource->DrawWindowContents();
		}
		else 
		{
			ImGui::Text(state->resource->GetFileName().c_str());
			ImGui::SameLine();
			if (ImGui::Button("x"))
			{
				state->resource = nullptr;
			};
		}
	}
	if (stateAsShared && transitionIdSelected != -1)
	{
		const auto& it = stateAsShared->GetTransitions().find(transitionIdSelected);
		ImGui::Text("Transition");
		ImGui::Separator();
	}
	ImGui::EndChild();
	ImGui::SameLine();

    static ImVec2 scrolling(0.0f, 0.0f);

	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
    if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
    if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    // Draw border and background color
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

	// Pan (we use a zero mouse threshold when there's no context menu)
	// You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
	const float mouse_threshold_for_pan = 0.0f;
	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Middle, mouse_threshold_for_pan))
	{
		scrolling.x += io.MouseDelta.x;
		scrolling.y += io.MouseDelta.y;
	}

	const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
	const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);
    // Draw grid
    draw_list->PushClipRect(ImVec2(canvas_p0.x + 1, canvas_p0.y + 1), ImVec2(canvas_p1.x - 1, canvas_p1.y - 1), true);
    const float GRID_STEP = 64.0f;
    for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
        draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
    for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
        draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));

	//New Elements
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)
		&& ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && !ImGui::IsAnyItemActive())
	{
		stateIdSelected = -1;
		transitionIdSelected = -1;
		creatingTransition = false;
		openContextMenu = true;
	}

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)
		&& ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && !ImGui::IsAnyItemActive())
	{
		creatingTransition = false;
	}

	if (stateAsShared)
	{
		//Draw Actual TransitionCreation
		if(creatingTransition)
		{
			ImVec2 posState = ImVec2(stateAsShared->GetStates()[stateIdSelected]->auxiliarPos.first, stateAsShared->GetStates()[stateIdSelected]->auxiliarPos.second);
			ImVec2 posStateOriginCenter =
				ImVec2(origin.x + posState.x + sizeState.x/2, origin.y + posState.y + sizeState.y/2);
			ImVec2 postStateDestinationCenter =
				ImVec2(mouse_pos_in_canvas.x + origin.x, mouse_pos_in_canvas.y + origin.y);
			draw_list->AddLine(posStateOriginCenter, postStateDestinationCenter, IM_COL32(255, 255, 255, 255));
		}
		
		//Draw Transitions
		for(const auto& it : stateAsShared->GetTransitions())
		{
			Transition transition = it.second;
			ImGui::PushID(it.first);
			ImVec2 posStateOriginCenter = 
				ImVec2(origin.x + transition.origin->auxiliarPos.first + sizeState.x/2, origin.y + transition.origin->auxiliarPos.second + sizeState.y/2);
			ImVec2 postStateDestinationCenter = 
				ImVec2(origin.x + transition.destination->auxiliarPos.first + sizeState.x / 2, origin.y + transition.destination->auxiliarPos.second + sizeState.y / 2);

			float triangleLength = 15;
			float triangleHeight = 15;
			
			LineSegment2D line(float2(posStateOriginCenter.x, posStateOriginCenter.y), float2(postStateDestinationCenter.x, postStateDestinationCenter.y));

			float2 perpendicular = line.Dir().Perp();
			ImVec2 exitPoint = ImVec2(posStateOriginCenter.x + perpendicular.x * 6, posStateOriginCenter.y + perpendicular.y * 6);
			ImVec2 enterPoint = ImVec2(postStateDestinationCenter.x + perpendicular.x * 6, postStateDestinationCenter.y + perpendicular.y * 6);
			line = LineSegment2D(float2(exitPoint.x, exitPoint.y), float2(enterPoint.x, enterPoint.y));
			float2 center = line.CenterPoint();

			float2 p1 = center - (triangleLength * line.Dir()) / 2.f + triangleHeight / 2.f * perpendicular;
			float2 p2 = center - (triangleLength * line.Dir()) / 2.f - triangleHeight / 2.f * perpendicular;
			float2 p3 = center + (triangleLength * line.Dir()) / 2.f;

			ImVec2 p1Im = ImVec2(p1.x, p1.y);
			ImVec2 p2Im = ImVec2(p2.x, p2.y);
			ImVec2 p3Im = ImVec2(p3.x, p3.y);

			draw_list->AddLine(exitPoint, enterPoint, IM_COL32(255, 255, 255, 255));
			draw_list->AddTriangleFilled(p1Im, p2Im, p3Im, IM_COL32(255, 255, 255, 255));

			ImVec2 pCenter = ImVec2(center.x - triangleLength / 2.f, center.y - triangleHeight / 2.f);

			ImGui::SetCursorScreenPos(pCenter);
			ImGui::InvisibleButton("state", ImVec2(triangleLength, triangleHeight));

			if (ImGui::IsItemActive() || ImGui::IsItemHovered())
			{
				if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
				{
					if (creatingTransition)
					{
						transitionIdSelected = -1;
						creatingTransition = false;
					}
					else
					{
						openContextMenu = true;
						transitionIdSelected = it.first;
					}
				}
				if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				{
					stateIdSelected = -1;
					transitionIdSelected = it.first;
					creatingTransition = false;
				}
			}
			ImGui::PopID();
		}

		//Draw States
		for (int i = 0; i < stateAsShared->GetNumStates(); i++)
		{
			State* state = stateAsShared->GetStates()[i];
			ImGui::PushID(i);
			ImVec2 posState = ImVec2(state->auxiliarPos.first, state->auxiliarPos.second);
			ImVec2 minRect = ImVec2(origin.x + posState.x, origin.y + posState.y);
			ImVec2 maxRect = ImVec2(origin.x + posState.x + sizeState.x, origin.y + posState.y + sizeState.y);
			ImGui::SetCursorScreenPos(minRect);
			ImGui::InvisibleButton("state", ImVec2(sizeState.x, sizeState.y));
			if (ImGui::IsItemActive() || ImGui::IsItemHovered())
			{
				if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				{
					if(creatingTransition)
					{
						if (stateIdSelected == i) 
						{
							transitionIdSelected = -1;
							creatingTransition = false;
						}
						else
						{
							stateAsShared->AddNewTransition(stateIdSelected, i);
							creatingTransition = false;
							transitionIdSelected = -1;
						}
					}
					else
					{
						transitionIdSelected = -1;
						stateIdSelected = i;
					}
				}
				if(ImGui::IsMouseDragging(ImGuiMouseButton_Left))
				{
					state->auxiliarPos = std::make_pair<int, int>(posState.x + io.MouseDelta.x, posState.y + io.MouseDelta.y);
					//stateAsShared->EditState(i, state);
				}
				if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
				{
					stateIdSelected = i;
					openContextMenu = true;
					if(creatingTransition)
					{
						transitionIdSelected = -1;
						creatingTransition = false;
					}
				}
			}

			draw_list->AddRectFilled(
				ImVec2(minRect.x - 2, minRect.y - 2), ImVec2(maxRect.x + 2, maxRect.y + 2),
				IM_COL32(100, 100, 120, 255), 
				4.0f
			);
			draw_list->AddRectFilledMultiColor(
				minRect, maxRect,
				IM_COL32(65, 65, 75, 255), IM_COL32(65, 65, 75, 255),
				IM_COL32(25, 25, 45, 255), IM_COL32(25, 25, 45, 255)
			);
			ImGui::SetNextItemWidth(150);
			ImGui::SetCursorScreenPos(ImVec2(minRect.x + 25, minRect.y + 10));
			ImGui::BeginGroup(); // Lock horizontal position
			ImGui::Text(state->name.c_str());
			std::string resource;
			state->resource != nullptr ? resource = state->resource->GetFileName().c_str() : resource = "Empty";
			ImGui::Text(resource.c_str());
			//INCLUDE RESOURCE SEARCH
			ImGui::EndGroup();
			ImGui::PopID();
		}
		draw_list->PopClipRect();
	}

	if(openContextMenu) ImGui::OpenPopup("RightClickStateMachine");

	if (ImGui::BeginPopup("RightClickStateMachine"))
	{
		openContextMenu = false;
		if(stateAsShared)
		{
			if (stateIdSelected >= 0 && stateIdSelected < stateAsShared->GetNumStates())
			{
				if (ImGui::MenuItem("Add Transition"))
				{
					creatingTransition = true;
					transitionIdSelected = stateAsShared->GetNumTransitions() - 1;
				}
				if (ImGui::MenuItem("Delete State"))
				{
					stateAsShared->EraseState(stateIdSelected);
					stateIdSelected = -1;
					transitionIdSelected = -1;
				}
			}
			else if (transitionIdSelected >= 0 && transitionIdSelected < stateAsShared->GetNumTransitions())
			{
				if (ImGui::MenuItem("Delete Transition"))
				{
					stateAsShared->EraseTransition(transitionIdSelected);
					stateIdSelected = -1;
					transitionIdSelected = -1;
				}
			}
			else
			{
				if (ImGui::MenuItem("Create State"))
				{
					stateAsShared->AddNewState(mouse_pos_in_canvas.x, mouse_pos_in_canvas.y);
				}
			}
		}
		
		ImGui::EndPopup();
	}
	ImGui::EndChild();
}

void WindowStateMachineEditor::SetResourceOnState(const std::shared_ptr<Resource>& resource)
{
	if (stateMachine && stateIdSelected != -1)
	{
		stateMachine->SetStateResource(stateIdSelected, resource);
	}
}