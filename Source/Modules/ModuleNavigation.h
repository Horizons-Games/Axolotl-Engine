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

	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;

	void ChangeNavMesh(UID navMeshId);
	void BakeNavMesh(); // Builds new navMesh

	void DrawGizmos();	   // Draws NavMesh Gizmos
	ResourceNavMesh& GetNavMesh(); // Returns navMesh
	void Raycast(float3 startPosition,
				 float3 targetPosition,
				 bool& hitResult,
				 float3& hitPosition); // Shoots a raycast between startPosition and targetPosition and detects if
									   // there's a wall/obstacle between. Hitresult is set to true and hitPosition is
									   // the position where the wall has been detected
	void GetNavMeshHeightInPosition(const float3 position, float& height);

	bool CleanUp() override;

public:
	UID navMeshId = 0;

private:
	ResourceNavMesh navMesh;
};
