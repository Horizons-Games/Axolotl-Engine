#pragma once

enum class ModuleType { BASE, SPAWN, POSITION, COLOR };

class EmitterInstance;
class ParticleEmitter;

class ParticleModule
{
public:
	ParticleModule(ModuleType type, ParticleEmitter* emitter);
	virtual ~ParticleModule();

	virtual void Spawn(EmitterInstance* instance) {};
	virtual void Update(EmitterInstance* instance) {};
	
	void Save();
	void Load();

	const bool IsEnabled() const;
	const ModuleType GetType() const;

	void SetEnabled(bool enabled);

	virtual void DrawImGui() {}; //ImGui section drawing
	virtual void DrawDD(EmitterInstance* instance) {};

protected:
	ModuleType type;
	ParticleEmitter* emitter;

	bool enabled;
};

inline const bool ParticleModule::IsEnabled() const
{
	return enabled;
}

inline const ModuleType ParticleModule::GetType() const
{
	return type;
}

inline void ParticleModule::SetEnabled(bool enabled)
{
	this->enabled = enabled;
}
