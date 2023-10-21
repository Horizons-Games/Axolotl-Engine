#pragma once
#include "Auxiliar/Generics/Updatable.h"
#include "Component.h"

#include "Math/float3.h"
#include "DetourTileCache/DetourTileCache.h"

class ComponentTransform;

class ComponentObstacle: public Component, public Updatable
{
public:
	ComponentObstacle(bool active, GameObject* owner);
	~ComponentObstacle();

	void Update() override;

	void AddObstacle(); // If NavMesh and TileCache are built, Removes the current Obstacle and Adds it to the TileCache (NavMesh). Will be added as Box or as Cylinder depending on this.type.
	void RemoveObstacle(); // If NavMesh and TileCache are built, Removes the current Obstacle and obstacleReference is set to null

	void SetRadius(float newRadius);			
	float GetRadius() const;		
	void SetHeight(float newHeight);			
	float GetHeight() const;		
	void SetBoxSize(float3 size);				
	float3 GetBoxSize() const;
	void SetObstacleType(ObstacleType newType);
	int GetObstacleType() const;
	void SetDrawGizmo(bool value);
	bool GetDrawGizmo() const;
	float GetYOffset() const;
	void SetYOffset(float value);

	void ResetSize();

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	dtObstacleRef* obstacleReference = nullptr; // Obstacle reference in the TileCache (NavMesh)
	float3 boxSize = float3::one;
	float radius = 1.0f;
	float height = 2.0f;
	float yOffset = 0.0f;
	float3 currentPosition = float3::zero;
	float3 currentRotation = float3::zero;
	int obstacleType = ObstacleType::DT_OBSTACLE_BOX;
	bool mustBeDrawnGizmo = true;
	bool shouldAddObstacle = true;
	ComponentTransform * transform;

};

inline float ComponentObstacle::GetRadius() const
{
	return radius;
}

inline float ComponentObstacle::GetHeight() const
{
	return height;
}

inline float3 ComponentObstacle::GetBoxSize() const
{
	return boxSize;
}

inline int ComponentObstacle::GetObstacleType() const
{
	return obstacleType;
}

inline bool ComponentObstacle::GetDrawGizmo() const
{
	return mustBeDrawnGizmo;
}

inline float ComponentObstacle::GetYOffset() const
{
	return yOffset;
}