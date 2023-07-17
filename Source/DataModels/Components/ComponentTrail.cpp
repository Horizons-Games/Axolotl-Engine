#include "StdAfx.h"
#include "ComponentTrail.h"
#include "ComponentTransform.h"
#include "Application.h"

#include "GameObject/GameObject.h"

#include "Trail/Trail.h"

#include"ImGui/imgui_color_gradient.h"

#include "FileSystem/ModuleResources.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/Json.h"

#include "Resources/ResourceTexture.h"


ComponentTrail::ComponentTrail(bool active, GameObject* owner) : Component(ComponentType::TRAIL, active, owner, true)
{ 
	trail = new Trail();
}

ComponentTrail::~ComponentTrail()
{
}

void ComponentTrail::Draw() const
{
	trail->Draw();
}

void ComponentTrail::Update()
{
	ComponentTransform* trans = GetOwner()->GetComponent<ComponentTransform>();
	trail->Update(trans->GetGlobalPosition(), trans->GetGlobalRotation());
}

void ComponentTrail::InternalSave(Json& meta)
{
	meta["duration"] = static_cast<float>(trail->GetDuration());
	meta["minDistance"] = static_cast<float>(trail->GetMinDistance());
	meta["width"] = static_cast<float>(trail->GetWidth());
	ImGradient* gradient = trail->GetGradient();
	meta["numberOfMarks"] = static_cast<int>(gradient->getMarks().size());
	std::list<ImGradientMark*> marks = gradient->getMarks();
	int i = 0;
	Json jsonColors = meta["ColorsGradient"];
	for (ImGradientMark* const& mark : marks)
	{
		jsonColors[i]["color_x"] = static_cast<float>(mark->color[0]);
		jsonColors[i]["color_y"] = static_cast<float>(mark->color[1]);
		jsonColors[i]["color_z"] = static_cast<float>(mark->color[2]);
		jsonColors[i]["color_w"] = static_cast<float>(mark->color[3]);
		jsonColors[i]["pos"] = static_cast<float>(mark->position);
		i++;
	}
	UID uid = 0;
	std::string assetPath = "";
	std::shared_ptr<ResourceTexture> texture = trail->GetTexture();
	if (texture)
	{
		uid = texture->GetUID();
		assetPath = texture->GetAssetsPath();
	}

	meta["materialUID"] = static_cast<UID>(uid);
	meta["assetPathMaterial"] = assetPath.c_str();
}

void ComponentTrail::InternalLoad(const Json& meta)
{
	trail->SetDuration(static_cast<float>(meta["duration"]));
	trail->SetMinDistance(static_cast<float>(meta["minDistance"]));
	trail->SetWidth(static_cast<float>(meta["width"]));
	int numberOfMarks = static_cast<int>(meta["numberOfMarks"]);
	ImGradient* gradient = trail->GetGradient();
	gradient->getMarks().clear();

	Json jsonColors = meta["ColorsGradient"];
	for (int i = 0; i < numberOfMarks; i++)
	{
		gradient->addMark(static_cast<float>(jsonColors[i]["pos"]),
						  ImColor(static_cast<float>(jsonColors[i]["color_x"]),
								  static_cast<float>(jsonColors[i]["color_y"]),
								  static_cast<float>(jsonColors[i]["color_z"]),
								  static_cast<float>(jsonColors[i]["color_a"])));
	}
	gradient->refreshCache();
	trail->SetGradient(gradient);

	std::string path = meta["assetPathMaterial"];
	bool materialExists = !path.empty() && App->GetModule<ModuleFileSystem>()->Exists(path.c_str());

	if (materialExists)
	{
		std::shared_ptr<ResourceTexture> texture =
			App->GetModule<ModuleResources>()->RequestResource<ResourceTexture>(path);

		if (texture)
		{
			trail->SetTexture(texture);
		}
	}
}
