#pragma once
#include "Module.h"
#include "Resources/ResourceNavMesh.h"
#include "Math/float3.h"

class ModuleNavigation : public Module
{
public:
	ModuleNavigation();
	~ModuleNavigation() override;

	bool Init() override;
	bool Start() override;

	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;

	void SaveOptions(Json& meta);
	void LoadOptions(Json& meta);

	void BakeNavMesh(); // Builds new navMesh

	void DrawGizmos();	   // Draws NavMesh Gizmos
	std::shared_ptr<ResourceNavMesh> GetNavMesh(); // Returns navMesh
	void SetNavMesh(const std::shared_ptr<ResourceNavMesh> resource);
	void Raycast(float3 startPosition,
				 float3 targetPosition,
				 bool& hitResult,
				 float3& hitPosition); // Shoots a raycast between startPosition and targetPosition and detects if
									   // there's a wall/obstacle between. Hitresult is set to true and hitPosition is
									   // the position where the wall has been detected
	void GetNavMeshHeightInPosition(const float3 position, float& height);

	bool CleanUp() override;

	bool GetDrawNavMesh() const;
	void SetDrawNavMesh(bool newDrawNavMesh);

public:
	UID navMeshId = 0;

private:
	std::shared_ptr<ResourceNavMesh> navMesh;
	bool agentsAdded;
	bool drawNavMesh;
};

inline bool ModuleNavigation::GetDrawNavMesh() const
{
	return drawNavMesh;
}

inline void ModuleNavigation::SetDrawNavMesh(bool newDrawNavMesh)
{
	drawNavMesh = newDrawNavMesh;
}