#include "StdAfx.h"

#include "ModuleNavigation.h"

#include "Application.h"
#include "DataModels/Components/ComponentAgent.h"
#include "Detour/DetourCommon.h"
#include "DetourCrowd/DetourCrowd.h"
#include "DetourTileCache/DetourTileCache.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "fileSystem/ModuleResources.h"

ModuleNavigation::ModuleNavigation()
{
}

ModuleNavigation::~ModuleNavigation()
{
}

bool ModuleNavigation::Init()
{
	return true;
}

bool ModuleNavigation::Start()
{
	/*ModuleResources* moduleResources = App->GetModule<ModuleResources>();
	moduleResources->CreateDefaultResource(ResourceType::NavMesh, "navMesh");
	navMesh = moduleResources->RequestResource<ResourceNavMesh>("Assets/NavMesh/navMesh.nav");*/

	return true;
}

bool ModuleNavigation::CleanUp()
{
	// WIP: This should be done when playMode is stopped
	/*Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	for (ComponentAgent* agent : currentScene->GetAgentComponents())
	{
		agent->RemoveAgentFromCrowd();
	}*/

	return true;
}

UpdateStatus ModuleNavigation::PreUpdate()
{
	// WIP: This should be done when playMode starts
	if (App->IsOnPlayMode() && !agentsAdded)
	{
		Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();
		currentScene->UpdateSceneAgentComponents();

		for (ComponentAgent* agent : currentScene->GetAgentComponents())
		{
			agent->AddAgentToCrowd();
		}
		agentsAdded = true;
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleNavigation::Update()
{
	if (navMesh == nullptr || !navMesh->IsGenerated())
	{
		return UpdateStatus::UPDATE_CONTINUE;
	}

	navMesh->GetTileCache()->update(App->GetDeltaTime(), navMesh->GetNavMesh()); // Update obstacles
	navMesh->GetCrowd()->update(App->GetDeltaTime(), nullptr);					 // Update agents

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleNavigation::PostUpdate()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleNavigation::SaveOptions(Json& meta)
{
	if (navMesh)
	{
		Json jsonNavMesh = meta["NavMesh"];

		jsonNavMesh["navMeshUID"] = static_cast<UID>(navMesh->GetUID());
		jsonNavMesh["assetPathNavMesh"] = navMesh->GetAssetsPath().c_str();
	}
}

void ModuleNavigation::LoadOptions(Json& meta)
{
	Json jsonNavMesh = meta["NavMesh"];
	if (jsonNavMesh.Size() > 0)
	{
		std::shared_ptr<ResourceNavMesh> resourceNavMesh;
#ifdef ENGINE
		std::string path = jsonNavMesh["assetPathNavMesh"];
		bool resourceExists = path != "" && App->GetModule<ModuleFileSystem>()->Exists(path.c_str());
		if (resourceExists)
		{
			resourceNavMesh = App->GetModule<ModuleResources>()->RequestResource<ResourceNavMesh>(path);
		}
#else
		UID uid = jsonNavMesh["navMeshUID"];
		resourceNavMesh = App->GetModule<ModuleResources>()->SearchResource<ResourceNavMesh>(uid);

#endif
		if (resourceNavMesh)
		{
			SetNavMesh(resourceNavMesh);
		}
	}
}

void ModuleNavigation::ChangeNavMesh(UID navMeshId_)
{
	// App->resources->DecreaseReferenceCount(navMeshId);
	navMeshId = navMeshId_;
	// App->resources->IncreaseReferenceCount(navMeshId);
}

void ModuleNavigation::BakeNavMesh()
{
	/*MSTimer timer;
	timer.Start();*/
	LOG_DEBUG("Baking NavMesh");
	bool generated = navMesh->Build(App->GetModule<ModuleScene>()->GetLoadedScene());
	// unsigned timeMs = timer.Stop();
	if (generated)
	{
		navMesh->SetChanged(true);
		App->GetModule<ModuleResources>()->ReimportResource(navMesh->GetUID());

		navMesh->GetTileCache()->update(App->GetDeltaTime(), navMesh->GetNavMesh());
		navMesh->GetCrowd()->update(App->GetDeltaTime(), nullptr);

		LOG_DEBUG("NavMesh successfully baked");
	}
	else
	{
		LOG_ERROR("NavMesh ERROR. Could not be baked");
	}
}

void ModuleNavigation::DrawGizmos()
{
	navMesh->DrawGizmos(App->GetModule<ModuleScene>()->GetLoadedScene());
}

std::shared_ptr<ResourceNavMesh> ModuleNavigation::GetNavMesh()
{
	return navMesh;
}

void ModuleNavigation::SetNavMesh(const std::shared_ptr<ResourceNavMesh> resource)
{
	navMesh = resource;
}

void ModuleNavigation::Raycast(float3 startPosition, float3 targetPosition, bool& hitResult, float3& hitPosition)
{
	hitResult = false;
	hitPosition = startPosition;

	if (!navMesh->IsGenerated())
		return;

	dtNavMeshQuery* navQuery = navMesh->GetNavMeshQuery();
	if (navQuery == nullptr)
		return;

	float3 polyPickExt = float3(2, 4, 2);
	dtQueryFilter filter;
	filter.setIncludeFlags(0xffff ^ 0x10); // SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED
	filter.setExcludeFlags(0);
	filter.setAreaCost(0, 1.0f);  // SAMPLE_POLYAREA_GROUND
	filter.setAreaCost(1, 10.0f); // SAMPLE_POLYAREA_WATER
	filter.setAreaCost(2, 1.0f);  // SAMPLE_POLYAREA_ROAD
	filter.setAreaCost(3, 1.0f);  // SAMPLE_POLYAREA_DOOR
	filter.setAreaCost(4, 2.0f);  // SAMPLE_POLYAREA_GRASS
	filter.setAreaCost(5, 1.5f);  // SAMPLE_POLYAREA_JUMP

	dtPolyRef startRef;
	navQuery->findNearestPoly(startPosition.ptr(), polyPickExt.ptr(), &filter, &startRef, 0);

	if (startRef)
	{
		float t = 0;
		int npolys = 0;
		static const int MAX_POLYS = 256;

		dtPolyRef polys[MAX_POLYS];

		float3 hitNormal;

		navQuery->raycast(startRef,
						  startPosition.ptr(),
						  targetPosition.ptr(),
						  &filter,
						  &t,
						  hitNormal.ptr(),
						  polys,
						  &npolys,
						  MAX_POLYS);
		if (t > 1)
		{
			// No hit
			dtVcopy(hitPosition.ptr(), targetPosition.ptr());
			hitResult = false;
		}
		else
		{
			// Hit
			dtVlerp(hitPosition.ptr(), startPosition.ptr(), targetPosition.ptr(), t);
			hitResult = true;
		}
	}
}

void ModuleNavigation::GetNavMeshHeightInPosition(const float3 position, float& height)
{
	if (!navMesh->IsGenerated())
		return;

	dtNavMeshQuery* navQuery = navMesh->GetNavMeshQuery();
	if (navQuery == nullptr)
		return;

	float3 polyPickExt = float3(2, 4, 2);
	dtQueryFilter filter;
	filter.setIncludeFlags(0xffff ^ 0x10); // SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED
	filter.setExcludeFlags(0);
	filter.setAreaCost(0, 1.0f);  // SAMPLE_POLYAREA_GROUND
	filter.setAreaCost(1, 10.0f); // SAMPLE_POLYAREA_WATER
	filter.setAreaCost(2, 1.0f);  // SAMPLE_POLYAREA_ROAD
	filter.setAreaCost(3, 1.0f);  // SAMPLE_POLYAREA_DOOR
	filter.setAreaCost(4, 2.0f);  // SAMPLE_POLYAREA_GRASS
	filter.setAreaCost(5, 1.5f);  // SAMPLE_POLYAREA_JUMP

	dtPolyRef startRef;
	navQuery->findNearestPoly(position.ptr(), polyPickExt.ptr(), &filter, &startRef, 0);

	if (startRef)
		navQuery->getPolyHeight(startRef, position.ptr(), &height);
}
