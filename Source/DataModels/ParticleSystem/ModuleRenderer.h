#pragma once

#include "ParticleModule.h"

class ModuleRenderer : public ParticleModule
{
public:
	enum class Alignment { SCREEN, WORLD, AXIAL };

public:
	ModuleRenderer(ParticleEmitter* emitter);
	~ModuleRenderer();

	void Spawn(EmitterInstance* instance) override;
	void Update(EmitterInstance* instance) override;

	void DrawImGui() override;

	void SetAlignment(Alignment alignment);

	Alignment GetAlignment() const;

private:
	Alignment alignment;

};

inline void ModuleRenderer::SetAlignment(Alignment alignment)
{
	this->alignment = alignment;
}

inline ModuleRenderer::Alignment ModuleRenderer::GetAlignment() const
{
	return alignment;
}
