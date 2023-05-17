#pragma once

enum class ModuleType { BASE, SPAWN, POSITION, COLOR };

class EmitterInstance;

class ParticleModule
{
public:
	ParticleModule(ModuleType type);
	virtual ~ParticleModule();

	virtual void Spawn(EmitterInstance* emitter) {};
	virtual void Update(EmitterInstance* emitter) {};
	
	void Save();
	void Load();

	const bool IsEnabled() const;
	const ModuleType GetType() const;

	void SetEnabled(bool enabled);

	//ImGui section drawing
	virtual void DrawImGui() {};
	
private:
	ModuleType type;

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
