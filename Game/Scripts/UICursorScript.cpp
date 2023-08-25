#include "StdAfx.h"

#include "UICursorScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/ComponentScript.h"


REGISTERCLASS(UICursorScript);

UICursorScript::UICursorScript() : Script(), cursor(nullptr)
{
	REGISTER_FIELD(cursor, GameObject*);
}

void UICursorScript::Start()
{
	cursorImage = cursor->GetComponent<ComponentImage>();
}

void UICursorScript::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();
	ComponentTransform2D* transform = cursorImage->GetOwner()->GetComponent<ComponentTransform2D>();
	transform->SetPosition(float3(input->GetMousePosition().x, input->GetMousePosition().y, 0));
	cursorImage->Draw();
}