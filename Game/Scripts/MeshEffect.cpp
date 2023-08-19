#include "MeshEffect.h"

#include "StdAfx.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentScript.h"
#include "GameObject/GameObject.h"

REGISTERCLASS(MeshEffect);

MeshEffect::MeshEffect() : Script()
{
}

void MeshEffect::Start()
{
	FillMeshes(owner);
}

void MeshEffect::Update(float deltaTime)
{
}

void MeshEffect::ClearEffect()
{
	for (auto mesh : meshes)
	{
		mesh->SetDiscard(false);
		mesh->SetEffectColor(float3(0.f, 0.f, 0.f));
	}
}

void MeshEffect::FillMeshes(GameObject* parent)
{
	ComponentMeshRenderer* mesh = parent->GetComponentInternal<ComponentMeshRenderer>();
	if (mesh)
	{
		meshes.push_back(mesh);
	}

	for (GameObject* child : parent->GetChildren())
	{
		FillMeshes(child);
	}
}

void MeshEffect::EffectDiscard()
{
	for (auto mesh : meshes)
	{
		mesh->SetEffectColor(float3(0.f, 0.f, 0.f));
		mesh->SetDiscard(true);
	}
}

void MeshEffect::EffectColor(float3 color)
{
	for (auto mesh : meshes)
	{
		mesh->SetDiscard(false);
		mesh->SetEffectColor(color);
	}
}