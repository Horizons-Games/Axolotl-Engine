#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentMeshRenderer;

class MeshEffect : public Script
{
public:
	MeshEffect();
	~MeshEffect() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void FillMeshes(GameObject* parent);

	void ReserveSpace(int space);
	void AddColor(float4 color);
	void AddColors(std::vector<float4> colors);
	void CleanColors();

	void StartEffect(float maxTime, float timeBetween);
	void ClearEffect();

	void DamageEffect();

	void DisappearBodyEffect();

	void EnableDisableMeshes(bool enable);

	void SetTimeBetween(float timeBetween);
	
	void SetMaxTime(float maxTime);

private:
	void EffectDiscard();
	void EffectColor(float4 color);

	std::vector<ComponentMeshRenderer*> meshes;

	std::vector<float4> colors;
	bool activateEffect;
	float timeBetween;
	float maxTime;
	float effectTime;
};

inline void MeshEffect::ReserveSpace(int space)
{
	colors.reserve(space);
}

inline void MeshEffect::AddColor(float4 color)
{
	colors.push_back(color);
}

inline void MeshEffect::AddColors(std::vector<float4> colors)
{
	this->colors = colors;
}

inline void MeshEffect::CleanColors()
{
	colors.clear();
}

inline void MeshEffect::SetTimeBetween(float timeBetween)
{
	this->timeBetween = timeBetween;
}

inline void MeshEffect::SetMaxTime(float maxTime)
{
	this->maxTime = maxTime;
}