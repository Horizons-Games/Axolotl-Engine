#include "StdAfx.h"
#include "ParticleBillboardAssistance.h"
#include "GameObject/GameObject.h"
#include "Components/ComponentTransform.h"
#include "Application.h"
#include "ModulePlayer.h"

REGISTERCLASS(ParticleBillboardAssistance);

ParticleBillboardAssistance::ParticleBillboardAssistance() : Script() , blockY(false)
{
	REGISTER_FIELD(blockY, bool);
}

void ParticleBillboardAssistance::Start()
{
	ownerTransform = owner->GetComponent<ComponentTransform>();
}

void ParticleBillboardAssistance::Update(float deltaTime)
{
	UpdateTransform();
}

void ParticleBillboardAssistance::UpdateTransform() 
{
	ComponentTransform* objective = App->GetModule<ModulePlayer>()->GetCameraPlayerObject()->GetComponent<ComponentTransform>();
	float3 vecTowards = (objective->GetGlobalPosition() - ownerTransform->GetGlobalPosition()).Normalized();

	if (blockY) 
	{
		vecTowards.y = 0.0f;
	}

	Quat rot = Quat::LookAt(ownerTransform->GetGlobalForward().Normalized(), vecTowards, ownerTransform->GetGlobalUp().Normalized(), float3::unitY);
	Quat rotation = ownerTransform->GetGlobalRotation();
	Quat targetRotation = rot * rotation;

	Quat rotationError = targetRotation * rotation.Normalized().Inverted();
	rotationError.Normalize();

	if (!rotationError.Equals(Quat::identity, 0.05f))
	{
		ownerTransform->SetGlobalRotation(targetRotation);
		ownerTransform->RecalculateLocalMatrix();
		ownerTransform->UpdateTransformMatrices();
	}
}