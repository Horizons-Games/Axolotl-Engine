#include "UIImageControl.h"

#include "ModuleInput.h"
#include "Components/UI/ComponentImage.h"

REGISTERCLASS(UIImageControl);

UIImageControl::UIImageControl() : Script(), disableImgObject(nullptr), enableImgObject(nullptr), imageComponent(nullptr),
check(false), enableImgObject02(nullptr), enableImgObject03(nullptr), enableImgObject04(nullptr)
{
	REGISTER_FIELD(enableImgObject, GameObject*);
	REGISTER_FIELD(disableImgObject, GameObject*);
	//hardcode to delete on other VS
	REGISTER_FIELD(check, bool);
	REGISTER_FIELD(enableImgObject02, GameObject*);
	REGISTER_FIELD(enableImgObject03, GameObject*);
	REGISTER_FIELD(enableImgObject04, GameObject*);
}

void UIImageControl::Start()
{
	imageComponent = owner->GetComponent<ComponentImage>();
}

void UIImageControl::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();

	if (enableImgObject != nullptr)
	{
		if (powerupStatus == false && check == true)
		{
			if (input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN)
			{
				powerupStatus = true;
				enableImgObject->Enable();
			}
		}
		if (powerupStatus == true && powerupTimer <= 15.0f && check == true)
		{
			powerupTimer += deltaTime;
			if (powerupTimer >= 3.75f)
			{
				enableImgObject->Disable();
				enableImgObject02->Enable();
			}
			else if (powerupTimer >= 3.75f*2) 
			{
				enableImgObject02->Disable();
				enableImgObject03->Enable();
			}
			else if (powerupTimer >= 3.75f*3) 
			{
				enableImgObject03->Disable();
				enableImgObject04->Enable();
			}
		}
		else if (powerupStatus == true && powerupTimer >= 15.0f && check == true)
		{
			enableImgObject04->Disable();
			LOG_VERBOSE("Entra con un tiempo de {}", powerupTimer);
			powerupTimer = 0.0f;
			powerupStatus = false;
		}

		//This is to show the complete HUD just hardcode
		if (input->GetKey(SDL_SCANCODE_H) == KeyState::DOWN)
		{
			if (hudStatus == false)
			{
				hudStatus = true;
				enableImgObject->Enable();
				LOG_VERBOSE("Complete HUD ON");
			}
			else
			{
				hudStatus = false;
				enableImgObject->Disable();
				LOG_VERBOSE("Complete HUD OFF");
			}
		}
	}
}
