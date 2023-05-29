#include "UIImageControl.h"

REGISTERCLASS(UIImageControl);

UIImageControl::UIImageControl() : Script(), DisableImgObject(nullptr), EnableImgObject(nullptr), ImageComponent(nullptr),
check(false), EnableImgObject02(nullptr), EnableImgObject03(nullptr), EnableImgObject04(nullptr)
{
	REGISTER_FIELD(EnableImgObject, GameObject*);
	REGISTER_FIELD(DisableImgObject, GameObject*);
	//hardcode to delete
	REGISTER_FIELD(check, bool);
	REGISTER_FIELD(EnableImgObject02, GameObject*);
	REGISTER_FIELD(EnableImgObject03, GameObject*);
	REGISTER_FIELD(EnableImgObject04, GameObject*);
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
		if (powerupStatus == false && check == true)
		{
			if (input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN)
			{
				powerupStatus = true;
				EnableImgObject->Enable();
			}
		}
		if (powerupStatus == true && powerupTimer <= 15.0f && check == true)
		{
			powerupTimer += deltaTime;
			if (powerupTimer >= 3.75f)
			{
				EnableImgObject->Disable();
				EnableImgObject02->Enable();
			}
			if (powerupTimer >= 3.75f*2) 
			{
				EnableImgObject02->Disable();
				EnableImgObject03->Enable();
			}
			if (powerupTimer >= 3.75f*3) 
			{
				EnableImgObject03->Disable();
				EnableImgObject04->Enable();
			}
		}
		else if (powerupStatus == true && powerupTimer >= 15.0f && check == true)
		{
			EnableImgObject04->Disable();
			ENGINE_LOG("Entra con un tiempo de %f", powerupTimer);
			powerupTimer = 0.0f;
			powerupStatus = false;
		}

		//This is to show the complete HUD just hardcode
		if (input->GetKey(SDL_SCANCODE_H) == KeyState::DOWN)
		{
			if (hudStatus == false)
			{
				hudStatus = true;
				EnableImgObject->Enable();
				ENGINE_LOG("Complete HUD ON");
			}
			else
			{
				hudStatus = false;
				EnableImgObject->Disable();
				ENGINE_LOG("Complete HUD OFF");
			}
		}
	}
}
