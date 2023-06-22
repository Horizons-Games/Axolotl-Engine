#include "ModuleNavigation.h"
#include "Application.h"
#include "ModuleScene.h"
#include "DetourTileCache/DetourTileCache.h"
#include "DetourCrowd/DetourCrowd.h"
#include "Detour/DetourCommon.h"

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
	return true;
}

bool ModuleNavigation::CleanUp()
{
	return true;
}

update_status ModuleNavigation::PreUpdate()
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleNavigation::Update()
{
	if (!navMesh->IsGenerated())
	{
		return update_status::UPDATE_CONTINUE;
	}

	navMesh->GetTileCache()->update(App->GetDeltaTime(), navMesh->GetNavMesh()); // Update obstacles
	navMesh->GetCrowd()->update(App->GetDeltaTime(), nullptr);					// Update agents

	return update_status::UPDATE_CONTINUE;
}


update_status ModuleNavigation::PostUpdate()
{
	return update_status::UPDATE_CONTINUE;
}

void ModuleNavigation::ChangeNavMesh(UID navMeshId_)
{
	//App->resources->DecreaseReferenceCount(navMeshId);
	navMeshId = navMeshId_;
	//App->resources->IncreaseReferenceCount(navMeshId);
}

void ModuleNavigation::BakeNavMesh()
{
	/*MSTimer timer;
	timer.Start();*/
	ENGINE_LOG("Loading NavMesh");
	bool generated = navMesh->Build(App->GetModule<ModuleScene>()->GetLoadedScene());
	//unsigned timeMs = timer.Stop();
	if (generated)
	{
		navMesh->GetTileCache()->update(App->GetDeltaTime(), navMesh->GetNavMesh());
		navMesh->GetCrowd()->update(App->GetDeltaTime(), nullptr);

		ENGINE_LOG("NavMesh successfully baked");
	}
	else
	{
		ENGINE_LOG("NavMesh ERROR. Could not be baked");
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
