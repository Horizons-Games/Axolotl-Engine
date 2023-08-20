#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class GameObject;
class ComponentMeshRenderer;

class MeshEffect : public Script
{
public:
	MeshEffect();
	~MeshEffect() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void EffectDiscard();
	void EffectColor(float3 color);
	void ClearEffect();

private:
	void FillMeshes(GameObject* parent);

	std::vector<ComponentMeshRenderer*> meshes;
};