
#include "ComponentAnimation.h"
#include "ComponentTransform.h"

#include "Animation/AnimationController.h"

#include "FileSystem/Json.h"

#include "GameObject/GameObject.h"

ComponentAnimation::ComponentAnimation(const bool active, GameObject* owner)
	: Component(ComponentType::ANIMATION, active, owner, false) 
{
	controller = new AnimationController();
	animationIx = 0;
}

ComponentAnimation::~ComponentAnimation()
{
}

AnimationController* ComponentAnimation::GetController()
{
	return controller;
}

void ComponentAnimation::SetAnimations(std::vector<std::shared_ptr<ResourceAnimation>> animations)
{
	this->animations = animations;
	animationIx = 0;

	controller->Play(animations[animationIx], true);
}

void ComponentAnimation::Update()
{
	controller->Update();

	if (animations.size() > 0)
	{
		for (GameObject* child : owner->GetChildren())
		{
			float3 pos;
			Quat rot;

			if (controller->GetTransform(&child->GetName()[0], pos, rot))
			{
				ComponentTransform* transform =
					static_cast<ComponentTransform*>(child->GetComponent(ComponentType::TRANSFORM));
				transform->SetPosition(pos);
				transform->SetRotation(float4x4(rot));
			}
		}
	}
}

void ComponentAnimation::Draw()
{
}

void ComponentAnimation::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;
}

void ComponentAnimation::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
}