#pragma once

#include "EmitterInstance.h"
#include "ParticleModule.h"

class ModuleRenderer : public ParticleModule
{
public:
	enum class Alignment { SCREEN, WORLD, AXIAL };
	enum class BlendingMode { ALPHA, ADDITIVE };

public:
	ModuleRenderer(ParticleEmitter* emitter);
	~ModuleRenderer();

	void Spawn(EmitterInstance* instance) override;
	void Update(EmitterInstance* instance) override;

	void UpdateInstanceBuffer(EmitterInstance* instance);
	void DrawParticles(EmitterInstance* instance);
	void DrawImGui() override;

	void SetAlignment(const Alignment& alignment);
	void SetBlending(const BlendingMode& blending);

	Alignment GetAlignment() const;
	BlendingMode GetBlending() const;

private:
	Alignment alignment;
	BlendingMode blendingMode;

	// Render
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
	unsigned int instanceVbo;
	unsigned int numInstances;
};

inline void ModuleRenderer::SetAlignment(const Alignment& alignment)
{
	this->alignment = alignment;
}

inline void ModuleRenderer::SetBlending(const BlendingMode& blending)
{
	blendingMode = blending;
}

inline ModuleRenderer::Alignment ModuleRenderer::GetAlignment() const
{
	return alignment;
}

inline ModuleRenderer::BlendingMode ModuleRenderer::GetBlending() const
{
	return blendingMode;
}