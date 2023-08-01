#include "StdAfx.h"

#include "WindowComponentScript.h"

#include "Application.h"
#include "Scene/Scene.h"

#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/UIDGenerator.h"
#include "Modules/ModuleScene.h"

#include "Components/ComponentScript.h"

#include "IScript.h"
#include "ScriptFactory.h"

#include "Auxiliar/Reflection/VectorField.h"

namespace
{
const float doubleClickTimeFrameInS = .5f;
}

WindowComponentScript::WindowComponentScript(ComponentScript* component) :
	ComponentWindow("SCRIPT", component),
	windowUID(UniqueID::GenerateUID())
{
}

WindowComponentScript::~WindowComponentScript()
{
}

std::string WindowComponentScript::DrawStringField(std::string& value, const std::string& name)
{
	ImGui::InputText(name.c_str(), value.data(), 24);
	return value;
}

bool WindowComponentScript::DrawBoolField(bool& value, const std::string& name)
{
	ImGui::Checkbox(name.c_str(), &value);
	return value;
}

float WindowComponentScript::DrawFloatField(float& value, const std::string& name)
{
	ImGui::DragFloat(name.c_str(), &value, 0.05f, -50.0f, 50.0f, "%.2f");
	return value;
}

math::float3 WindowComponentScript::DrawFloat3Field(math::float3& value, const std::string& name)
{
	ImGui::DragFloat3(name.c_str(), (&value[2], &value[1], &value[0]), 0.05f, -50.0f, 50.0f, "%.2f");
	return value;
}

GameObject* WindowComponentScript::DrawGameObjectField(GameObject* value, const std::string& name)
{
	std::string gameObjectSlot = "Drag a GameObject here";
	if (value != nullptr)
	{
		gameObjectSlot = value->GetName().c_str();
	}

	ImGui::Button((gameObjectSlot + "##" + std::to_string(windowUID)).c_str(), ImVec2(208.0f, 20.0f));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY"))
		{
			UID draggedGameObjectID = *(UID*) payload->Data;
			GameObject* draggedGameObject =
				App->GetModule<ModuleScene>()->GetLoadedScene()->SearchGameObjectByID(draggedGameObjectID);

			if (draggedGameObject)
			{
				ImGui::EndDragDropTarget();
				return draggedGameObject;
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine(0.0f, 3.0f);
	ImGui::Text(name.c_str());
	ImGui::SameLine();
	if (ImGui::Button(("Remove" + name + "##" + std::to_string(windowUID)).c_str()))
	{
		return nullptr;
	}

	return value;
}

void WindowComponentScript::DrawWindowContents()
{
	// Vector of pairs instead of vector of ImRect, to avoid pulling "imgui_internal.h"
	std::vector<std::pair<ImVec2, ImVec2>> widgetRects;

	// Store the starting position of the collapsing header
	ImVec2 headerMinPos = ImGui::GetItemRectMin();

	DrawEnableAndDeleteComponent();

	// add the rect of the enable and delete buttons
	widgetRects.emplace_back(headerMinPos, ImGui::GetItemRectMax());

	ImGui::NewLine();

	std::vector<const char*> constructors = App->GetScriptFactory()->GetConstructors();
	ComponentScript* script = static_cast<ComponentScript*>(component);

	if (!script)
	{
		return;
	}

	std::string label = "Select Script";
	std::string separator = "##";
	std::string thisID = std::to_string(windowUID);
	std::string finalLabel = label + separator + thisID;

	const IScript* scriptObject = script->GetScript();

	if (!scriptObject)
	{
		if (ImGui::ListBox(
				finalLabel.c_str(), &currentItem, constructors.data(), static_cast<int>(constructors.size()), 5))
		{
			ChangeScript(script, constructors[currentItem]);
			LOG_VERBOSE("{} SELECTED, drawing its contents.", script->GetConstructName());
		}

		label = "Create Script##";
		finalLabel = label + thisID;
		if (ImGui::Button(finalLabel.c_str()))
		{
			ImGui::OpenPopup("Create new script");
		}

		ImGui::SetNextWindowSize(ImVec2(280, 75));

		if (ImGui::BeginPopupModal(
				"Create new script", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar))
		{
			OpenCreateNewScriptPopUp();
			ImGui::EndPopup();
		}
		return;
	}

	ImGui::Separator();

	std::string scriptName = script->GetConstructName().c_str();
	ImGui::Text(scriptName.c_str());

	// add the rect of the script name
	widgetRects.emplace_back(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

	if (ImGui::GetWindowWidth() > static_cast<float>(scriptName.size()) * 13.0f)
	{
		ImGui::SameLine(ImGui::GetWindowWidth() - 120.0f);
	}

	else
	{
		ImGui::SameLine();
	}

	label = "Remove Script##";
	finalLabel = label + thisID;
	if (ImGui::Button(finalLabel.c_str()))
	{
		LOG_VERBOSE("{} REMOVED, showing list of available scripts.", script->GetConstructName());

		script->SetScript(nullptr);			  // This deletes the script itself
		script->SetConstuctor(std::string()); // And this makes it so it's also deleted from the serialization
		return;
	}

	for (TypeFieldPair enumAndMember : scriptObject->GetFields())
	{
		ValidFieldType member = enumAndMember.second;

		switch (enumAndMember.first)
		{
			case FieldType::FLOAT:
			{
				const Field<float>& floatField = std::get<Field<float>>(member);
				float value = floatField.getter();

				label = floatField.name;
				finalLabel = label + separator + thisID;

				floatField.setter(DrawFloatField(value, finalLabel.c_str()));

				// add the rect of each field
				widgetRects.emplace_back(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
				break;
			}

			case FieldType::FLOAT3:
			{
				const Field<float3>& float3Field = std::get<Field<float3>>(member);
				float3 value = float3Field.getter();

				float3Field.setter(DrawFloat3Field(value, float3Field.name.c_str()));

				// add the rect of each field
				widgetRects.emplace_back(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
				break;
			}

			case FieldType::STRING:
			{
				const Field<std::string>& stringField = std::get<Field<std::string>>(member);
				std::string value = stringField.getter();

				label = stringField.name;
				finalLabel = label + separator + thisID;

				stringField.setter(DrawStringField(value, finalLabel).c_str());

				// add the rect of each field
				widgetRects.emplace_back(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
				break;
			}

			case FieldType::BOOLEAN:
			{
				const Field<bool>& booleanField = std::get<Field<bool>>(member);
				bool value = booleanField.getter();

				label = booleanField.name;
				finalLabel = label + separator + thisID;

				booleanField.setter(DrawBoolField(value, finalLabel.c_str()));

				// add the rect of each field
				widgetRects.emplace_back(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
				break;
			}

			case FieldType::GAMEOBJECT:
			{
				const Field<GameObject*>& gameObjectField = std::get<Field<GameObject*>>(member);
				GameObject* value = gameObjectField.getter();

				GameObject* draggedObject = DrawGameObjectField(value, gameObjectField.name);

				gameObjectField.setter(draggedObject);

				// add the rect of each field
				widgetRects.emplace_back(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
				break;
			}

			case FieldType::VECTOR:
			{
				const VectorField& vectorField = std::get<VectorField>(member);

				std::function<std::any(std::any&, const std::string&)> elementDrawer =
					[this, &vectorField](std::any& value, const std::string& name) -> std::any
				{
					switch (vectorField.innerType)
					{
						case FieldType::FLOAT:
							if (!value.has_value())
							{
								value = std::make_any<float>();
							}
							return float(DrawFloatField(std::any_cast<float&>(value), name));
						case FieldType::STRING:
							if (!value.has_value())
							{
								value = std::make_any<std::string>();
							}
							return std::string(DrawStringField(std::any_cast<std::string&>(value), name).c_str());
						case FieldType::BOOLEAN:
							if (!value.has_value())
							{
								value = std::make_any<bool>();
							}
							return bool(DrawBoolField(std::any_cast<bool&>(value), name));
						case FieldType::GAMEOBJECT:
							if (!value.has_value())
							{
								value = std::make_any<GameObject*>();
							}
							return std::any(DrawGameObjectField(std::any_cast<GameObject*>(value), name));
						case FieldType::FLOAT3:
							if (!value.has_value())
							{
								value = std::make_any<float3>(0.f);
							}
							return float3(DrawFloat3Field(std::any_cast<float3&>(value), name));
					}
					return std::any(); // Default return
				};

				std::vector<std::any> vectorValue = vectorField.getter();

				{
					ImVec2 startingPos = ImGui::GetCursorScreenPos();

					ImGui::Text(vectorField.name.c_str());
					ImGui::SameLine();
					if (ImGui::Button(("+##" + vectorField.name).c_str()))
					{
						vectorValue.emplace_back();
					}
					ImGui::SameLine();
					if (ImGui::Button(("-##" + vectorField.name).c_str()) && !vectorValue.empty())
					{
						vectorValue.pop_back();
					}

					widgetRects.emplace_back(startingPos, ImGui::GetItemRectMax());
				}

				for (int i = 0; i < vectorValue.size(); ++i)
				{
					ImGui::Indent();
					ImVec2 startingPos = ImGui::GetCursorScreenPos();
					vectorValue[i] = elementDrawer(vectorValue[i], vectorField.name + std::to_string(i));
					// add the rect of each field
					widgetRects.emplace_back(startingPos, ImGui::GetItemRectMax());
					ImGui::Unindent();
				}

				vectorField.setter(vectorValue);

				break;
			}

			default:
				break;
		}
	}

	// Store the ending position of the collapsing header
	ImVec2 headerMaxPos = ImGui::GetItemRectMax();

	// Adjust headerMin and headerMax for the full width of the header
	headerMinPos.x = ImGui::GetWindowPos().x;
	headerMaxPos.x = ImGui::GetWindowPos().x + ImGui::GetWindowWidth();

	secondsSinceLastClick += App->GetDeltaTime();

	bool noWidgetHovered = std::none_of(std::begin(widgetRects),
										std::end(widgetRects),
										[](const std::pair<ImVec2, ImVec2>& rect)
										{
											return ImGui::IsMouseHoveringRect(rect.first, rect.second);
										});

	if (noWidgetHovered && ImGui::IsMouseHoveringRect(headerMinPos, headerMaxPos) &&
		ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		if (IsDoubleClicked())
		{
			std::string scriptPath = "Scripts\\" + script->GetConstructName() + ".cpp";
			ShellExecuteA(0, 0, scriptPath.c_str(), 0, 0, SW_SHOW);
		}
		secondsSinceLastClick = 0;
	}
}

void WindowComponentScript::ChangeScript(ComponentScript* newScript, const char* selectedScript)
{
	newScript->SetConstuctor(selectedScript);
	IScript* Iscript = App->GetScriptFactory()->ConstructScript(selectedScript);
	Iscript->SetOwner(component->GetOwner());
	newScript->SetScript(Iscript);
}

void WindowComponentScript::OpenCreateNewScriptPopUp()
{
	static char name[FILENAME_MAX] = "NewScript";
	ImGui::InputText("Script Name", name, IM_ARRAYSIZE(name));
	ImGui::NewLine();

	ImGui::SameLine(ImGui::GetWindowWidth() - 120);
	if (ImGui::Button("Save", ImVec2(50, 20)))
	{
		AddNewScriptToProject(name);
		// reset the input string contents
		strcpy_s(name, "NewScript");
		ImGui::CloseCurrentPopup();
	}

	ImGui::SameLine(ImGui::GetWindowWidth() - 60);
	if (ImGui::Button("Cancel"))
	{
		ImGui::CloseCurrentPopup();
	}
}

void WindowComponentScript::AddNewScriptToProject(const std::string& scriptName)
{
	std::string scriptsPath = "Scripts/";

	std::string scriptHeaderPath = scriptsPath + scriptName + ".h";
	std::string scriptSourcePath = scriptsPath + scriptName + ".cpp";

	const ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

	// Both header and source have the same name, so only checking the header is enough
	if (fileSystem->Exists(scriptHeaderPath.c_str()))
	{
		LOG_INFO("That name is already used by another script, please use a different one");
		return;
	}

	char* headerBuffer = nullptr;
	char* sourceBuffer = nullptr;

	unsigned int headerBufferSize = fileSystem->Load("Source/PreMades/TemplateHeaderScript", headerBuffer);
	unsigned int sourceBufferSize = fileSystem->Load("Source/PreMades/TemplateSourceScript", sourceBuffer);

	std::string headerString(headerBuffer, headerBufferSize);
	delete headerBuffer;
	std::string sourceString(sourceBuffer, sourceBufferSize);
	delete sourceBuffer;

	ReplaceSubstringsInString(headerString, "{0}", scriptName);
	ReplaceSubstringsInString(sourceString, "{0}", scriptName);

	fileSystem->Save(scriptHeaderPath.c_str(), headerString.c_str(), headerString.size());
	fileSystem->Save(scriptSourcePath.c_str(), sourceString.c_str(), sourceString.size());

	LOG_INFO("New script {} created", scriptName);
}

void WindowComponentScript::ReplaceSubstringsInString(std::string& stringToReplace,
													  const std::string& from,
													  const std::string& to)
{
	if (from.empty())
	{
		return;
	}

	size_t startPos = 0;
	while ((startPos = stringToReplace.find(from, startPos)) != std::string::npos)
	{
		stringToReplace.replace(startPos, from.length(), to);
		startPos += to.length();
	}
}

bool WindowComponentScript::IsDoubleClicked()
{
	return secondsSinceLastClick <= doubleClickTimeFrameInS;
}
