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

static const char* conditionNamesFloat[] = { "Greater", "Less", "Equal", "NotEqual" };
static const char* conditionNamesBool[] = { "True", "False"};
static int boolNamesOffset = IM_ARRAYSIZE(conditionNamesFloat);

WindowStateMachineEditor::WindowStateMachineEditor() : EditorWindow("State Machine Editor"),
stateIdSelected(-1), transitionIdSelected(-1), creatingTransition(false), openContextMenu(false), sizeState(200, 50), inputResource(std::make_unique<WindowResourceInput>())
{
}

void WindowStateMachineEditor::SetResourceOnState(const std::shared_ptr<Resource>& resource)
{
	std::shared_ptr<ResourceStateMachine> stateAsShared = stateMachine.lock();
	if (stateAsShared && stateIdSelected != -1)
	{
		stateAsShared->SetStateResource(stateIdSelected, resource);
	}
}

void WindowStateMachineEditor::DrawWindowContents()
{
	std::shared_ptr<ResourceStateMachine> stateAsShared = stateMachine.lock();

	ImGui::BeginChild("Side_lists", ImVec2(300, 0));
	if (stateAsShared) 
	{
		ImGui::Text("Parameters");
		ImGui::SameLine();
		DrawAddParameterMenu(stateAsShared);
		ImGui::Separator();
		DrawParameters(stateAsShared);

		ImGui::Text("");

		DrawStateEditor(stateAsShared);

		DrawTransitionEditor(stateAsShared);
		
		if (ImGui::Button("Save"))
		{
			stateAsShared->SetChanged(true);
			App->GetModule<ModuleResources>()->ReimportResource(stateAsShared->GetUID());
		}
	}

	ImGui::EndChild();

	ImGui::SameLine();

	static ImVec2 scrolling(0.0f, 0.0f);

	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

	ImVec2 canvasP0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!

	// Draw border and background color
	ImGuiIO& io = ImGui::GetIO();
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	const ImVec2 origin(canvasP0.x + scrolling.x, canvasP0.y + scrolling.y); // Lock scrolled origin
	const ImVec2 mousePosInCanvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);
	
	DrawGridStateMachine(canvasP0, scrolling, io.MouseDelta, drawList);

	//Right and Left Click On the Void
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
		if(creatingTransition)
		{
			std::pair<int,int> posSelectedState = stateAsShared->GetState(stateIdSelected)->auxiliarPos;
			DrawActualTransitionCreation(ImVec2(posSelectedState.first, posSelectedState.second), origin, mousePosInCanvas, drawList);
		}
		
		DrawTransitions(stateAsShared, origin, drawList);

		DrawStates(stateAsShared, origin, io.MouseDelta, drawList);
	}

	drawList->AddRectFilled(
		ImVec2(canvasP0.x + 10, canvasP0.y + 10), ImVec2(canvasP0.x + 156, canvasP0.y + 30),
		IM_COL32(30, 30, 30, 255),
		0.0f
	);

	drawList->AddRect(
		ImVec2(canvasP0.x + 6, canvasP0.y + 6), ImVec2(canvasP0.x + 160, canvasP0.y + 34),
		IM_COL32(150, 150, 150, 255),
		0.0f
	);

	ImGui::SetCursorScreenPos(ImVec2(canvasP0.x + 15, canvasP0.y + 12));
	if (stateAsShared)
	{
		ImGui::Text(stateAsShared->GetFileName().c_str());
	}
	else 
	{
		ImGui::Text("No Machine Selected");
	}

	drawList->PopClipRect();

	DrawRightClickPopUp(stateAsShared, mousePosInCanvas);

	ImGui::EndChild();
}

void WindowStateMachineEditor::DrawAddParameterMenu(std::shared_ptr<ResourceStateMachine>& stateAsShared)
{
	if (ImGui::BeginMenu("+"))
	{
		if (ImGui::MenuItem("New Float"))
		{
			stateAsShared->AddParameter("NewFloat", FieldTypeParameter::FLOAT, 0.0f);
		}
		if (ImGui::MenuItem("New Bool"))
		{
			stateAsShared->AddParameter("NewBool", FieldTypeParameter::BOOL, false);
		}
		ImGui::EndMenu();
	}
}

void WindowStateMachineEditor::DrawParameters(std::shared_ptr<ResourceStateMachine>& stateAsShared)
{
	const std::string* oldName = nullptr;
	std::string newName;
	TypeFieldPairParameter field;
	for (const auto& it : stateAsShared->GetParameters())
	{
		std::string name = it.first;
		name.resize(24);
		ImGui::SetNextItemWidth(10);
		if (ImGui::Button(("x##" + name).c_str()))
		{
			stateAsShared->EraseParameter(name.c_str());
			break;
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(110);
		if (ImGui::InputText(("##NameParameter" + name).c_str(), &name[0], ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
		{
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)) && name != "")
			{
				oldName = &it.first;
				newName = name.c_str();
				field = it.second;
			}
		}
		ImGui::SameLine();
		ValidFieldTypeParameter value = it.second.second;
		ImGui::SetNextItemWidth(90);
		switch (it.second.first)
		{
		case FieldTypeParameter::FLOAT:
			if (ImGui::DragFloat(("##Float" + name).c_str(), &std::get<float>(value)))
			{
				stateAsShared->SetParameter(it.first, value);
			}
			break;
		case FieldTypeParameter::BOOL:
			if (ImGui::Checkbox(("##Bool" + name).c_str(), &std::get<bool>(value)))
			{
				stateAsShared->SetParameter(it.first, value);
			}
			break;
		default:
			break;
		}
	}

	if (newName != "")
	{
		stateAsShared->EraseParameter(*oldName);
		stateAsShared->AddParameter(newName.c_str(), field.first, field.second);
	}
}

void WindowStateMachineEditor::DrawStateEditor(std::shared_ptr<ResourceStateMachine>& stateAsShared)
{
	if (stateIdSelected < stateAsShared->GetNumStates() && stateIdSelected > 0)
	{
		State* state = stateAsShared->GetState(stateIdSelected);
		ImGui::Text("State");
		ImGui::Separator();
		std::string name = state->name;
		name.resize(24);
		if (ImGui::InputText("##Name", &name[0], 24, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			stateAsShared->SetStateName(stateIdSelected, name.c_str());
		}

		if (state->resource == nullptr)
		{
			inputResource->DrawWindowContents();
		}
		else
		{
			ImGui::Checkbox("Loop", &state->loop);

			ImGui::Text(state->resource->GetFileName().c_str());
			ImGui::SameLine();
			if (ImGui::Button("x"))
			{
				state->resource = nullptr;
			};
		}
	}
}

void WindowStateMachineEditor::DrawTransitionEditor(std::shared_ptr<ResourceStateMachine>& stateAsShared)
{
	if (transitionIdSelected != -1)
	{
		auto it = stateAsShared->GetTransitions().find(transitionIdSelected);
		ImGui::Text("Transition");
		ImGui::Separator();

		std::string title = stateAsShared->GetState(it->second.originState)->name.c_str();
		std::string title2 = stateAsShared->GetState(it->second.destinationState)->name.c_str();
		ImGui::Text((title + " -> " + title2).c_str());

		ImGui::Checkbox("Wait Exit", &it->second.waitUntilFinish);

		double duration = it->second.transitionDuration;
		if (ImGui::InputDouble("Duration", &duration))
		{
			stateAsShared->SetDurationTransition(transitionIdSelected, duration);
		}

		ImGui::Text("Conditions");
		ImGui::SameLine();
		if (ImGui::Button("+##2"))
		{
			stateAsShared->AddCondition(transitionIdSelected);
		}
		ImGui::Separator();
		for (int i = 0; i < it->second.conditions.size(); i++)
		{
			if(ImGui::Button(("X##" + std::to_string(i)).c_str()))
			{
				stateAsShared->EraseCondition(transitionIdSelected, i);
				break;
			}

			ImGui::SameLine();

			Condition& condition = it->second.conditions[i];
			ImGui::SetNextItemWidth(90);
			if (ImGui::BeginCombo(("##comboName" + std::to_string(i)).c_str(), condition.parameter.c_str()))
			{
				for (const auto& parameter : stateAsShared->GetParameters())
				{
					if (ImGui::Selectable(parameter.first.c_str()))
					{
						stateAsShared->SelectConditionParameter(parameter.first.c_str(), condition);
					}
				}
				ImGui::EndCombo();
			}
			ImGui::SameLine();

			if (condition.parameter != "")
			{
				const auto& itParameter = stateAsShared->GetParameters().find(condition.parameter);
				if (itParameter != stateAsShared->GetParameters().end())
				{
					ValidFieldTypeParameter valueCondition = condition.value;
					switch (itParameter->second.first)
					{
					case FieldTypeParameter::FLOAT:
						ImGui::SetNextItemWidth(90);
						if (ImGui::BeginCombo(("##comboCondition1" + std::to_string(i)).c_str(), conditionNamesFloat[static_cast<int>(condition.conditionType)]))
						{
							for (int i = 0; i < IM_ARRAYSIZE(conditionNamesFloat); i++)
							{
								if (ImGui::Selectable(conditionNamesFloat[i]))
								{
									stateAsShared->SelectCondition(static_cast<ConditionType>(i
										), condition);
								}
							}
							ImGui::EndCombo();
						}
						ImGui::SameLine();
						ImGui::SetNextItemWidth(90);
						if (ImGui::DragFloat(("##ConditionFloat" + std::to_string(i)).c_str(), &std::get<float>(valueCondition)))
						{
							stateAsShared->SelectConditionValue(valueCondition, condition);
						}
						break;
					case FieldTypeParameter::BOOL:
						ImGui::SetNextItemWidth(90);
						if (ImGui::BeginCombo(("##comboCondition2" + std::to_string(i)).c_str(), conditionNamesBool[static_cast<int>(condition.conditionType) - (boolNamesOffset)]))
						{
							for (int i = 0; i < IM_ARRAYSIZE(conditionNamesBool); i++)
							{
								if (ImGui::Selectable(conditionNamesBool[i]))
								{
									stateAsShared->SelectCondition(static_cast<ConditionType>(i + boolNamesOffset
										), condition);
								}
							}
							ImGui::EndCombo();
						}
						break;
					default:
						break;
					}
				}
				else
				{
					ImGui::Text("Parameter not found!");
				}

			}
		}
	}
}

void WindowStateMachineEditor::DrawGridStateMachine(const ImVec2& canvasP0, ImVec2& scrolling, const ImVec2& mouseDelta, ImDrawList* drawList)
{
	ImVec2 canvasSize = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
	if (canvasSize.x < 50.0f) canvasSize.x = 50.0f;
	if (canvasSize.y < 50.0f) canvasSize.y = 50.0f;
	ImVec2 canvasP1 = ImVec2(canvasP0.x + canvasSize.x, canvasP0.y + canvasSize.y);

	// Pan (we use a zero mouse threshold when there's no context menu)
	// You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
	const float mouseThresholdForPan = 0.0f;
	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Middle, mouseThresholdForPan))
	{
		scrolling.x += mouseDelta.x;
		scrolling.y += mouseDelta.y;
	}

	drawList->AddRectFilled(canvasP0, canvasP1, IM_COL32(50, 50, 50, 255));
	drawList->AddRect(canvasP0, canvasP1, IM_COL32(255, 255, 255, 255));

	// Draw grid
	drawList->PushClipRect(ImVec2(canvasP0.x + 1, canvasP0.y + 1), ImVec2(canvasP1.x - 1, canvasP1.y - 1), true);
	const float GRID_STEP = 64.0f;
	for (float x = fmodf(scrolling.x, GRID_STEP); x < canvasSize.x; x += GRID_STEP)
		drawList->AddLine(ImVec2(canvasP0.x + x, canvasP0.y), ImVec2(canvasP0.x + x, canvasP1.y), IM_COL32(200, 200, 200, 40));
	for (float y = fmodf(scrolling.y, GRID_STEP); y < canvasSize.y; y += GRID_STEP)
		drawList->AddLine(ImVec2(canvasP0.x, canvasP0.y + y), ImVec2(canvasP1.x, canvasP0.y + y), IM_COL32(200, 200, 200, 40));

}

void WindowStateMachineEditor::DrawActualTransitionCreation(const ImVec2& posState, const ImVec2& origin, const ImVec2& mousePosInCanvas, ImDrawList* drawList)
{
	ImVec2 posStateOriginCenter =
		ImVec2(origin.x + posState.x + sizeState.x / 2, origin.y + posState.y + sizeState.y / 2);
	ImVec2 postStateDestinationCenter =
		ImVec2(mousePosInCanvas.x + origin.x, mousePosInCanvas.y + origin.y);
	drawList->AddLine(posStateOriginCenter, postStateDestinationCenter, IM_COL32(255, 255, 255, 255));
}

void WindowStateMachineEditor::DrawTransitions(std::shared_ptr<ResourceStateMachine>& stateAsShared, const ImVec2& origin, ImDrawList* drawList)
{
	for (const auto& it : stateAsShared->GetTransitions())
	{
		Transition transition = it.second;
		State* stateOrigin = stateAsShared->GetState(transition.originState);
		State* stateDestination = stateAsShared->GetState(transition.destinationState);
		ImGui::PushID(it.first);
		ImVec2 posStateOriginCenter =
			ImVec2(origin.x + stateOrigin->auxiliarPos.first + sizeState.x / 2, origin.y + stateOrigin->auxiliarPos.second + sizeState.y / 2);
		ImVec2 postStateDestinationCenter =
			ImVec2(origin.x + stateDestination->auxiliarPos.first + sizeState.x / 2, origin.y + stateDestination->auxiliarPos.second + sizeState.y / 2);

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

		drawList->AddLine(exitPoint, enterPoint, IM_COL32(255, 255, 255, 255));
		drawList->AddTriangleFilled(p1Im, p2Im, p3Im, IM_COL32(255, 255, 255, 255));

		ImVec2 pCenter = ImVec2(center.x - triangleLength / 2.f, center.y - triangleHeight / 2.f);

		ImGui::SetCursorScreenPos(pCenter);
		ImGui::InvisibleButton(("transition" + std::to_string(it.first)).c_str(), ImVec2(triangleLength, triangleHeight));

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
}

void WindowStateMachineEditor::DrawStates(std::shared_ptr<ResourceStateMachine>& stateAsShared, const ImVec2& origin, const ImVec2& mouseDelta, ImDrawList* drawList)
{
	for (int i = 0; i < stateAsShared->GetNumStates(); i++)
	{
		State* state = stateAsShared->GetState(i);
		if(state != nullptr)
		{
			ImGui::PushID(i);
			ImVec2 posState = ImVec2(state->auxiliarPos.first, state->auxiliarPos.second);
			ImVec2 minRect = ImVec2(origin.x + posState.x, origin.y + posState.y);
			ImVec2 maxRect = ImVec2(origin.x + posState.x + sizeState.x, origin.y + posState.y + sizeState.y);
			ImGui::SetCursorScreenPos(minRect);
			ImGui::InvisibleButton(("state" + std::to_string(i)).c_str(), ImVec2(sizeState.x, sizeState.y));
			if (ImGui::IsItemActive() || ImGui::IsItemHovered())
			{
				if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				{
					if (creatingTransition)
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
				if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
				{
					state->auxiliarPos = std::make_pair<int, int>(posState.x + mouseDelta.x, posState.y + mouseDelta.y);
					//stateAsShared->EditState(i, state);
				}
				if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
				{
					stateIdSelected = i;
					openContextMenu = true;
					if (creatingTransition)
					{
						transitionIdSelected = -1;
						creatingTransition = false;
					}
				}
			}

			if (i != 0)
			{
				drawList->AddRectFilled(
					ImVec2(minRect.x - 2, minRect.y - 2), ImVec2(maxRect.x + 2, maxRect.y + 2),
					IM_COL32(100, 100, 120, 255),
					4.0f
				);
				drawList->AddRectFilledMultiColor(
					minRect, maxRect,
					IM_COL32(65, 65, 75, 255), IM_COL32(65, 65, 75, 255),
					IM_COL32(25, 25, 45, 255), IM_COL32(25, 25, 45, 255)
				);
			}
			else
			{
				drawList->AddRectFilled(
					ImVec2(minRect.x - 2, minRect.y - 2), ImVec2(maxRect.x + 2, maxRect.y + 2),
					IM_COL32(0, 120, 0, 255),
					4.0f
				);
				drawList->AddRectFilledMultiColor(
					minRect, maxRect,
					IM_COL32(0, 125, 0, 255), IM_COL32(0, 125, 0, 255),
					IM_COL32(0, 85, 0, 255), IM_COL32(0, 85, 0, 255)
				);
			}

			ImGui::SetNextItemWidth(150);
			ImGui::SetCursorScreenPos(ImVec2(minRect.x + 25, minRect.y + 10));
			ImGui::BeginGroup(); // Lock horizontal position
			ImGui::Text(state->name.c_str());
			std::string resource;
			state->resource != nullptr ? resource = state->resource->GetFileName().c_str() : resource = "Empty";
			if (i != 0) ImGui::Text(resource.c_str());
			ImGui::EndGroup();
			ImGui::PopID();
		}
	}
}

void WindowStateMachineEditor::DrawRightClickPopUp(std::shared_ptr<ResourceStateMachine>& stateAsShared, const ImVec2& mousePosInCanvas)
{
	if (openContextMenu) ImGui::OpenPopup("RightClickStateMachine");

	if (ImGui::BeginPopup("RightClickStateMachine"))
	{
		openContextMenu = false;
		if (stateAsShared)
		{
			if (stateIdSelected >= 0 && stateIdSelected < stateAsShared->GetNumStates())
			{
				if (ImGui::MenuItem("Add Transition"))
				{
					creatingTransition = true;
				}
				if (stateIdSelected != 0 && ImGui::MenuItem("Delete State"))
				{
					stateAsShared->EraseState(stateIdSelected);
					stateIdSelected = -1;
					transitionIdSelected = -1;
				}
			}
			else if (transitionIdSelected != -1)
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
					stateAsShared->AddNewState(mousePosInCanvas.x, mousePosInCanvas.y);
				}
			}
		}

		ImGui::EndPopup();
	}
}

