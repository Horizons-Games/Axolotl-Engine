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

	void SetColor(const float4& color);
	
	float4 Getcolor() const;

private:
	float4 color;

	ImGradient gradient;
	ImGradientMark* draggingMark = nullptr;
	ImGradientMark* selectedMark = nullptr;
};

inline void ModuleColor::SetColor(const float4& color)
{
	this->color = color;
}

inline float4 ModuleColor::Getcolor() const
{
	return color;
}


