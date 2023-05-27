#include "ComponentParticleSystem.h"
#include "FileSystem/UniqueID.h"
#include "Resources/ResourceParticleSystem.h"
#include "ParticleSystem/EmitterInstance.h"
#include "FileSystem/Json.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "ParticleSystem/ParticleEmitter.h"

ComponentParticleSystem::ComponentParticleSystem(const bool active, GameObject* owner) :
	Component(ComponentType::PARTICLE, active, owner, true), resource(nullptr)
{
}

ComponentParticleSystem::~ComponentParticleSystem()
{
	ClearEmitters();
}

void ComponentParticleSystem::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;

	UID uidParticleSystem = 0;
	std::string assetPath = "";

	if (resource)
	{
		uidParticleSystem = resource->GetUID();
		assetPath = resource->GetAssetsPath();
	}

	meta["particleSystemUID"] = (UID)uidParticleSystem;
	meta["assetPathParticleSystem"] = assetPath.c_str();
}

void ComponentParticleSystem::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
	std::shared_ptr<ResourceParticleSystem> resourceParticleSystem;
#ifdef ENGINE
	std::string path = meta["assetPathParticleSystem"];
	bool resourceExists = path != "" && App->GetModule<ModuleFileSystem>()->Exists(path.c_str());
	if (resourceExists)
	{
		resourceParticleSystem = App->GetModule<ModuleResources>()->RequestResource<ResourceParticleSystem>(path);
	}
#else
	UID uidParticleSystem = meta["particleSystemUID"];
	resourceParticleSystem = App->GetModule<ModuleResources>()->SearchResource<ResourceParticleSystem>(uidParticleSystem);

#endif
	if (resourceParticleSystem)
	{
		SetResource(resourceParticleSystem);
	}
}

void ComponentParticleSystem::Update()
{
	for (EmitterInstance* emitter : emitters)
	{
		emitter->UpdateModules();
	}
}

void ComponentParticleSystem::Draw() const
{
	for (EmitterInstance* instance : emitters)
	{
		instance->DrawDD();
		instance->SimulateParticles();
	}
}

void ComponentParticleSystem::Reset()
{
}

void ComponentParticleSystem::CreateEmitterInstance()
{
	EmitterInstance* instance = new EmitterInstance(nullptr, this);
	instance->Init();
	emitters.push_back(instance);
}

void ComponentParticleSystem::CreateEmitterInstance(const ParticleEmitter* emitter)
{
	EmitterInstance* instance = new EmitterInstance(emitter, this);
	instance->Init();
	emitters.push_back(instance);
}

void ComponentParticleSystem::AddEmitterInstance(EmitterInstance* emitter)
{
	emitters.push_back(emitter);
}

void ComponentParticleSystem::SetResource(const std::shared_ptr<ResourceParticleSystem> resource)
{
	this->resource = resource;
	ClearEmitters();

	if(resource != nullptr)
	{
		InitEmitterInstances();
	}
}

void ComponentParticleSystem::CheckEmitterInstances()
{
	if (resource == nullptr) 
	{
		ClearEmitters();
		return;
	}

	if (emitters.size() != resource->GetNumEmitters())
	{
		ClearEmitters();
		InitEmitterInstances();
	}
}

void ComponentParticleSystem::InitEmitterInstances()
{
	emitters.resize(resource->GetNumEmitters());
	for(int i = 0; i < resource->GetNumEmitters(); ++i)
	{
		CreateEmitterInstance(resource->GetEmitter(i));
	}
}

void ComponentParticleSystem::ClearEmitters()
{
	for (EmitterInstance* instance : emitters)
	{
		delete instance;
	}

	emitters.clear();
}