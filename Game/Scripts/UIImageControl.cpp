#include "UIImageControl.h"

REGISTERCLASS(UIImageControl);

UIImageControl::UIImageControl() : Script(), DisableImgObject(nullptr), EnableImgObject(nullptr), ImageComponent(nullptr),
check(false)
{
	REGISTER_FIELD(EnableImgObject, GameObject*);
	REGISTER_FIELD(DisableImgObject, GameObject*);
	REGISTER_FIELD(check, bool);
}

void UIImageControl::Start()
{
	ImageComponent = static_cast<ComponentImage*>(owner->GetComponent(ComponentType::IMAGE));
}

void UIImageControl::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();

	if (EnableImgObject != nullptr)
	{
		if (powerupStatus != true && check == true)
		{
			if (input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN)
			{
				powerupTimer = time + 0.005;
				powerupStatus = true;
				EnableImgObject->Enable();
			}
		}
		else if (powerupTimer <= deltaTime)
		{
			powerupTimer = 0;
			powerupStatus = false;
			EnableImgObject->Disable();
		}

		if (input->GetKey(SDL_SCANCODE_H) == KeyState::DOWN)
		{
			if (hudStatus == false)
			{
				hudStatus = true;
				EnableImgObject->Enable();
			}
			else
			{
				hudStatus = false;
				EnableImgObject->Disable();
			}
			
		}
	}
}
