#include "StdAfx.h"
#include "ExplosionManagerEffect.h"

#include "Application.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentParticleSystem.h"
#include "Components/ComponentPointLight.h"
#include "Components/ComponentMeshRenderer.h"
#include "Batch/GeometryBatch.h"
#include "GameObject/GameObject.h"

REGISTERCLASS(ExplosionManagerEffect);

ExplosionManagerEffect::ExplosionManagerEffect() : Script(), initFase(true), actualPhase(0), sphereGrowFactor(0.20f), lightGrowFactor(2.0f)
{
	REGISTER_FIELD(sphereGrowFactor, float);
	REGISTER_FIELD(lightGrowFactor, float);
	REGISTER_FIELD(timerPhases, std::vector<float>);
}

ExplosionManagerEffect::~ExplosionManagerEffect()
{

}

void ExplosionManagerEffect::Start()
{
	phase1 = owner->GetChildren()[1];
	phase2 = owner->GetChildren()[2];
	light = static_cast<ComponentPointLight*>(owner->GetChildren()[0]->GetComponent<ComponentLight>());
}

void ExplosionManagerEffect::Update(float deltaTime)
{
	if (actualPhase > timerPhases.size()) 
	{
		return;
	}

	switch (actualPhase)
	{
	case 0:
	{
		//Poner bola, luz y sparks y crecen ligeramente, sobretodo la luz se intensifica a algo naranjoso
		ComponentTransform* sphere = phase1->GetChildren()[0]->GetComponent<ComponentTransform>();
		ComponentMeshRenderer* sphereMesh = sphere->GetOwner()->GetComponent<ComponentMeshRenderer>();
		if (initFase)
		{
			sphere->GetOwner()->GetParent()->Enable();
			ComponentParticleSystem* particlesGO = phase1->GetChildren()[1]->GetComponent<ComponentParticleSystem>();
			particlesGO->Enable();
			particlesGO->Play();
			light->GetOwner()->Enable();
			initFase = false;
		}

		float3 scale = sphere->GetLocalScale();
		scale = scale.Add((deltaTime * sphereGrowFactor));
		sphere->SetLocalScale(scale);
		sphere->UpdateTransformMatrices();

		float4 diffuseColor = sphereMesh->GetDiffuseColor();
		diffuseColor.y -= (deltaTime * lightGrowFactor/4);
		if (diffuseColor.y < 0.0f) diffuseColor.y = 0.0f;
		diffuseColor.w -= (deltaTime * lightGrowFactor/4);
		sphereMesh->SetDiffuseColor(diffuseColor);
		sphereMesh->GetBatch()->SetFillMaterials(true);

		break;
	}
	case 1:
	{
		//Bola y sparks van reduciendose y tornandose alpha y naranja hasta desaparecer, la luz tambien se vuelve mas suave y naranja/rojo
		//Entran sombra en el suelo del impacto, algunas lineas de acción y una expansión muy transparente
		ComponentTransform* sphere = phase1->GetChildren()[0]->GetComponent<ComponentTransform>();
		ComponentMeshRenderer* sphereMesh = sphere->GetOwner()->GetComponent<ComponentMeshRenderer>();
		if (initFase)
		{
			light->GetOwner()->Disable();
			ComponentParticleSystem* particleGO = phase2->GetChildren()[0]->GetComponent<ComponentParticleSystem>();
			particleGO->GetOwner()->Enable();
			particleGO->Enable();
			particleGO->Play();
			initFase = false;
		}

		float3 scale = sphere->GetLocalScale();
		scale = scale.Add(-(deltaTime * sphereGrowFactor / 4));
		sphere->SetLocalScale(scale);
		sphere->UpdateTransformMatrices();

		float4 diffuseColor = sphereMesh->GetDiffuseColor();
		diffuseColor.y -= (deltaTime * lightGrowFactor);
		if (diffuseColor.y < 0.0f)
		{
			diffuseColor.y = 0.0f;
		}
		diffuseColor.w -= (deltaTime * lightGrowFactor);
		sphereMesh->SetDiffuseColor(diffuseColor);
		sphereMesh->GetBatch()->SetFillMaterials(true);

		break;
	}
	case 2:

		if (initFase) 
		{
			phase1->GetChildren()[0]->Disable();
			initFase = false;
		}
		break;
	default:
		break;
	}

	if (countTime >= timerPhases[actualPhase]) 
	{
		++actualPhase;
		countTime = 0.0f;
		initFase = true;
	}
	else 
	{
		countTime += deltaTime;
	}
}