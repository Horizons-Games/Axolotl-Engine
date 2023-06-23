#pragma once

#include "EmitterInstance.h"
#include "ParticleModule.h"
#include "ParticleEmitter.h"

class ResourceTexture;
class WindowParticleTexture;

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
	void SetTexture(const std::shared_ptr<ResourceTexture>& texture);

	void SetTiles(int tilex, int tiley);
	void SetSheetSpeed(float sheetSpeed);
	void SetRandomFrame(bool randomFrame);
	void SetFrameBlending(bool frameBlending);

	Alignment GetAlignment() const;
	BlendingMode GetBlending() const;
	std::shared_ptr<ResourceTexture> GetTexture();

	std::pair<int,int> GetTiles() const;
	float GetSheetSpeed() const;
	bool GetRandomFrame() const;
	bool GetFrameBlending() const;

private:
	Alignment alignment;
	BlendingMode blendingMode;

	// Render
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
	unsigned int instanceVbo;
	unsigned int numInstances;

	WindowParticleTexture* windowTexture;
	
	// Texture sheet info
	int tiles[2] = { 1, 1 };
	float sheetSpeed = 0.0f;
	bool randomFrame = false;
	bool frameBlending = true;
};

inline void ModuleRenderer::SetAlignment(const Alignment& alignment)
{
	this->alignment = alignment;
}

inline void ModuleRenderer::SetBlending(const BlendingMode& blending)
{
	blendingMode = blending;
}

inline void ModuleRenderer::SetTexture(const std::shared_ptr<ResourceTexture>& texture)
{
	emitter->SetTexture(texture);
}

inline void ModuleRenderer::SetTiles(int tilex, int tiley)
{
	tiles[0] = tilex;
	tiles[1] = tiley;
}

inline void ModuleRenderer::SetSheetSpeed(float sheetSpeed)
{
	this->sheetSpeed = sheetSpeed;
}

inline void ModuleRenderer::SetRandomFrame(bool randomFrame)
{
	this->randomFrame = randomFrame;
}

inline void ModuleRenderer::SetFrameBlending(bool frameBlending)
{
	this->frameBlending = frameBlending;
}

inline ModuleRenderer::Alignment ModuleRenderer::GetAlignment() const
{
	return alignment;
}

inline ModuleRenderer::BlendingMode ModuleRenderer::GetBlending() const
{
	return blendingMode;
}

inline std::shared_ptr<ResourceTexture> ModuleRenderer::GetTexture()
{
	return emitter->GetParticleTexture();
}

inline std::pair<int, int> ModuleRenderer::GetTiles() const
{
	return std::pair<int,int>(tiles[0], tiles[1]);
}

inline float ModuleRenderer::GetSheetSpeed() const
{
	return sheetSpeed;
}

inline bool ModuleRenderer::GetRandomFrame() const
{
	return randomFrame;
}

inline bool ModuleRenderer::GetFrameBlending() const
{
	return frameBlending;
}
