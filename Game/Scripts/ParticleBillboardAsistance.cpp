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
	ComponentTransform* objective = App->GetModule<ModulePlayer>()->GetCameraPlayerObject()->GetComponent<ComponentTransform>();
	float3 vecTowards = (objective->GetGlobalPosition() - ownerTransform->GetGlobalPosition()).Normalized();

	vecTowards.y = 0;
	vecTowards = vecTowards.Normalized();

	Quat rot = Quat::LookAt(ownerTransform->GetGlobalForward().Normalized(), vecTowards, float3::unitY, float3::unitY);
	ownerTransform->SetGlobalRotation(rot);
	ownerTransform->RecalculateLocalMatrix();
}