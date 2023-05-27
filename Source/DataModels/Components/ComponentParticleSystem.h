#pragma once

#include "Component.h"

#include "Auxiliar/Generics/Drawable.h"
#include "Auxiliar/Generics/Updatable.h"

#include <vector>
#include <memory>

class EmitterInstance;
class ParticleEmitter;
class ResourceParticleSystem;

class ComponentParticleSystem : public Component, public Drawable, public Updatable
{
public:
	ComponentParticleSystem(const bool active, GameObject* owner);
	~ComponentParticleSystem() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void Update() override;
	void Reset();
	void Draw() const override;

	bool IsEmittersEmpty() const;
	std::vector<EmitterInstance*> GetEmitters() const;

	const std::shared_ptr<ResourceParticleSystem>& GetResource() const;
	void SetResource(const std::shared_ptr<ResourceParticleSystem> resource);

	void CheckEmitterInstances();

private:
	void CreateEmitterInstance();
	void CreateEmitterInstance(ParticleEmitter* emitter);
	void AddEmitterInstance(EmitterInstance* emitter);

	void ClearEmitters();
	void InitEmitterInstances();

	std::vector<EmitterInstance*> emitters;
	std::shared_ptr<ResourceParticleSystem> resource;
};

inline bool ComponentParticleSystem::IsEmittersEmpty() const
{
	return emitters.empty();
}

inline const std::shared_ptr<ResourceParticleSystem>& ComponentParticleSystem::GetResource() const
{
	return resource;
}

inline std::vector<EmitterInstance*> ComponentParticleSystem::GetEmitters() const
{
	return emitters;
}
