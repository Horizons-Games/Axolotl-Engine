#pragma once

#include "ParticleModule.h"

#include "Math/float4.h"

#include "ImGui/imgui_color_gradient.h"

class ModuleColor : public ParticleModule
{
public:
	ModuleColor(ParticleEmitter* emitter);
	~ModuleColor() override;

	void Spawn(EmitterInstance* emitter) override;
	void Update(EmitterInstance* emitter) override;

	void DrawImGui() override;

private:
	float initAlpha;
	float endAlpha;

	ImGradient gradient;
	ImGradientMark* draggingMark;
	ImGradientMark* selectedMark;
};


