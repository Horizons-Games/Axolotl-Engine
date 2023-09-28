#include "StdAfx.h"
#include "ParticleBillboardAsistance.h"
#include "GameObject/GameObject.h"
#include "Components/ComponentTransform.h"
#include "Application.h"
#include "ModulePlayer.h"

REGISTERCLASS(ParticleBillboardAsistance);

ParticleBillboardAsistance::ParticleBillboardAsistance() : Script()
{
}

void ParticleBillboardAsistance::Start()
{
	ownerTransform = owner->GetComponent<ComponentTransform>();
}

void ParticleBillboardAsistance::Update(float deltaTime)
{
	UpdateTransform();
}

void ParticleBillboardAsistance::UpdateTransform() 
{
	ComponentTransform* objective = App->GetModule<ModulePlayer>()->GetCameraPlayerObject()->GetComponent<ComponentTransform>();
	float3 vecTowards = (objective->GetGlobalPosition() - ownerTransform->GetGlobalPosition()).Normalized();

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