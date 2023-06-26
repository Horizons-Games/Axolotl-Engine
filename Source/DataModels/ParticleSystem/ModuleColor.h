#pragma once

#include "ParticleModule.h"

#include "Math/float4.h"

#include "ImGui/imgui_color_gradient.h"

class ModuleColor : public ParticleModule
{
public:
	ModuleColor(ParticleEmitter* emitter);
	~ModuleColor() override;

	void Spawn(EmitterInstance* instance) override;
	void Update(EmitterInstance* instance) override;

	void DrawImGui() override;

	float GetInitAlpha() const;
	float GetEndAlpha() const;
	ImGradient* GetGradient() const;

	void SetInitAlpha(float initAlpha);
	void SetEndAlpha(float endAlpha);
	void SetGradient(ImGradient* gradient);

private:
	float initAlpha;
	float endAlpha;

	ImGradient* gradient;
	ImGradientMark* draggingMark;
	ImGradientMark* selectedMark;
};

inline float ModuleColor::GetInitAlpha() const
{
	return initAlpha;
}

inline float ModuleColor::GetEndAlpha() const
{
	return endAlpha;
}

inline ImGradient* ModuleColor::GetGradient() const
{
	return gradient;
}

inline void ModuleColor::SetInitAlpha(float initAlpha)
{
	this->initAlpha = initAlpha;
}

inline void ModuleColor::SetEndAlpha(float endAlpha)
{
	this->endAlpha = endAlpha;
}

inline void ModuleColor::SetGradient(ImGradient* gradient)
{
	this->gradient = gradient;
}
