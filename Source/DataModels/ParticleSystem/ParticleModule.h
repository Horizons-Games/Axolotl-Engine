#pragma once

enum class ModuleType { BASE, SPAWN, POSITION, COLOR };

class EmitterInstance;

class ParticleModule
{
public:
	ParticleModule(ModuleType type);
	virtual ~ParticleModule();

	virtual void Spawn(EmitterInstance*);
	virtual void Update(EmitterInstance*);
	
	void Save();
	void Load();

	const ModuleType GetType() const;
	
private:
	ModuleType type;
};

inline const ModuleType ParticleModule::GetType() const
{
	return type;
}