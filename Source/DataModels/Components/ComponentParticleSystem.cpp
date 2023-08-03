#include "StdAfx.h"

#include "Application.h"
#include "ComponentParticleSystem.h"
#include "ComponentTransform.h"
#include "ModuleScene.h"

#include "Camera/Camera.h"

#include "FileSystem/UIDGenerator.h"
#include "FileSystem/Json.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"

#include "Modules/ModuleCamera.h"
#include "Modules/ModuleProgram.h"

#include "ParticleSystem/EmitterInstance.h"
#include "ParticleSystem/ParticleEmitter.h"

#include "Program/Program.h"

#include "Resources/ResourceParticleSystem.h"

#include "Scene/Scene.h"


ComponentParticleSystem::ComponentParticleSystem(const bool active, GameObject* owner) :
	Component(ComponentType::PARTICLE, active, owner, true), 
	resource(nullptr),
	isPlaying(false),
	pause(false),
	playAtStart(false)
{
}

ComponentParticleSystem::ComponentParticleSystem(const ComponentParticleSystem& toCopy):
	Component(ComponentType::PARTICLE, toCopy.IsEnabled(), toCopy.GetOwner(), true),
	isPlaying(false),
	pause(false),
	playAtStart(toCopy.playAtStart)
{ 
	SetResource(toCopy.GetResource());
}

ComponentParticleSystem::~ComponentParticleSystem()
{
	ClearEmitters();
}

void ComponentParticleSystem::InternalSave(Json& meta)
{

	UID uidParticleSystem = 0;
	std::string assetPath = "";

	if (resource)
	{
		uidParticleSystem = resource->GetUID();
		assetPath = resource->GetAssetsPath();
	}

	meta["particleSystemUID"] = (UID)uidParticleSystem;
	meta["assetPathParticleSystem"] = assetPath.c_str();
	meta["playAtStart"] = playAtStart;
}

void ComponentParticleSystem::InternalLoad(const Json& meta)
{
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

	playAtStart = meta["playAtStart"];
}

void ComponentParticleSystem::Play()
{
	isPlaying = true;
	
	for (EmitterInstance* emitter : emitters)
	{
		emitter->Init();
	}
}

void ComponentParticleSystem::Pause()
{
	pause = !pause;
}

void ComponentParticleSystem::Stop()
{
	isPlaying = false;
	pause = false;

	for (EmitterInstance* emitter : emitters)
	{
		emitter->Stop();
	}
}

void ComponentParticleSystem::Update()
{
	if (IsEnabled() && isPlaying && !pause)
	{
		for (EmitterInstance* emitter : emitters)
		{
			emitter->UpdateModules();
		}
	}
}

void ComponentParticleSystem::Draw() const
{
	for (EmitterInstance* instance : emitters)
	{
#ifdef ENGINE
		if (!App->IsOnPlayMode())
		{
			instance->DrawDD();
			//instance->SimulateParticles();
			//Draw the BoundingBox of ComponentParticle
			ComponentTransform* transform = GetOwner()->GetComponent<ComponentTransform>();
			if (transform->IsDrawBoundingBoxes())
			{
				App->GetModule<ModuleDebugDraw>()->DrawBoundingBox(transform->GetObjectOBB());
			}
		}
#endif //ENGINE
	}
}

void ComponentParticleSystem::Render()
{
	ModuleCamera* camera = App->GetModule<ModuleCamera>();
	ComponentTransform* transform = GetOwner()->GetComponent<ComponentTransform>();
	if (IsEnabled() &&	camera->GetCamera()->IsInside(transform->GetEncapsuledAABB()))
	{
		Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::PARTICLES);

		program->Activate();

		const float4x4& view = camera->GetCamera()->GetViewMatrix();
		const float4x4& proj = camera->GetCamera()->GetProjectionMatrix();

		program->BindUniformFloat4x4(0, reinterpret_cast<const float*>(&proj), true);
		program->BindUniformFloat4x4(1, reinterpret_cast<const float*>(&view), true);

		for (EmitterInstance* instance : emitters)
		{
			instance->DrawParticles();
		}

		program->Deactivate();
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

void ComponentParticleSystem::CreateEmitterInstance(ParticleEmitter* emitter)
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
	ComponentTransform* transform = GetOwner()->GetComponent<ComponentTransform>();
	//set the origin to translate and scale the BoundingBox
	transform->SetOriginScaling({0.5,0.5,0.5});
	transform->SetOriginCenter({ 0.0, 0.0, 0.0 });

	//Apply the BoundingBox modification
	float3 translation = transform->GetBBPos();
	float3 scaling = transform->GetBBScale();
	transform->TranslateLocalAABB(translation);
	transform->ScaleLocalAABB(scaling);
	transform->CalculateBoundingBoxes();
}

void ComponentParticleSystem::CheckEmitterInstances(bool forceRecalculate)
{
	if (resource == nullptr)
	{
		ClearEmitters();
		return;
	}

	if (forceRecalculate || emitters.size() != resource->GetNumEmitters())
	{
		ClearEmitters();
		InitEmitterInstances();
	}
}

void ComponentParticleSystem::InitEmitterInstances()
{
	emitters.reserve(resource->GetNumEmitters());
	for(unsigned int i = 0; i < resource->GetNumEmitters(); ++i)
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

void ComponentParticleSystem::RemoveEmitter(int pos)
{
	delete emitters[pos];
	emitters.erase(emitters.begin() + pos);
}
