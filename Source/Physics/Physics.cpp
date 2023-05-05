#include "Physics.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"

#include "Scene/Scene.h"
#include "GameObject/GameObject.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentMeshRenderer.h"

#include "DataStructures/Quadtree.h"
#include <DataModels/Resources/ResourceMesh.h>

#include "Windows/EditorWindows/WindowScene.h"

#include "Geometry/Frustum.h"
#include "Math/float2.h"
#include "Geometry/Triangle.h"
#include <queue>

float2 Physics::ScreenToScenePosition(const float2& mousePosition) 
{
#ifdef ENGINE
	// normalize the input to [-1, 1].
	const WindowScene* windowScene = App->GetModule<ModuleEditor>()->GetScene();
	ImVec2 startPosScene = windowScene->GetStartPos();
	ImVec2 endPosScene = windowScene->GetEndPos();
	if (!ImGuizmo::IsOver() && !windowScene->isMouseInsideManipulator(mousePosition.x, mousePosition.y))
	{
		if (mousePosition.x > startPosScene.x && mousePosition.x < endPosScene.x
			&& mousePosition.y > startPosScene.y && mousePosition.y < endPosScene.y)
		{
			float2 mousePositionAdjusted = mousePosition;
			mousePositionAdjusted.x -= startPosScene.x;
			mousePositionAdjusted.y -= startPosScene.y;

			return mousePositionAdjusted;
		}
	}
#endif
	return mousePosition;
}
bool Physics::ScreenPointToRay(const float2& mousePosition, LineSegment& ray)
{

#ifdef ENGINE
	// normalize the input to [-1, 1].
	const WindowScene* windowScene = App->GetModule<ModuleEditor>()->GetScene();
	ImVec2 startPosScene = windowScene->GetStartPos();
	ImVec2 endPosScene = windowScene->GetEndPos();
	if (!ImGuizmo::IsOver() && !windowScene->isMouseInsideManipulator(mousePosition.x, mousePosition.y))
	{
		if (mousePosition.x > startPosScene.x && mousePosition.x < endPosScene.x
			&& mousePosition.y > startPosScene.y && mousePosition.y < endPosScene.y)
		{
			float2 mousePositionAdjusted = mousePosition;
			mousePositionAdjusted.x -= startPosScene.x;
			mousePositionAdjusted.y -= startPosScene.y;

			float width = windowScene->GetAvailableRegion().x;
			float height = windowScene->GetAvailableRegion().y;

			float normalizedX = -1.0f + 2.0f * mousePositionAdjusted.x / width;
			float normalizedY = 1.0f - 2.0f * mousePositionAdjusted.y / height;


			ray = App->GetModule<ModuleCamera>()->GetCamera()->GetFrustum()->UnProjectLineSegment(normalizedX, normalizedY);
			return true;
		}
	}
#endif //ENGINE
	return false;
}

bool Physics::Raycast(const LineSegment& ray, RaycastHit& hit)
{
	std::map<float, const GameObject*> hitGameObjects;

#ifdef ENGINE
	AddIntersectionGameObject(hitGameObjects, ray, scene->GetSelectedGameObject());
#endif
	ModuleScene* scene = App->GetModule<ModuleScene>();
	AddIntersectionQuadtree(hitGameObjects, ray, scene->GetLoadedScene()->GetRootQuadtree());
	AddIntersectionDynamicObjects(hitGameObjects, ray, scene->GetLoadedScene()->GetNonStaticObjects());

	GetRaycastHitInfo(hitGameObjects, ray, hit);

	if (hit.gameObject != nullptr)
	{
		return true;
	}

	return false;
}

bool Physics::Raycast(const LineSegment& ray, RaycastHit& hit, GameObject* exceptionGameObject)
{
	std::map<float, const GameObject*> hitGameObjects;
	ModuleScene* scene = App->GetModule<ModuleScene>();

#ifdef ENGINE
	AddIntersectionGameObject(hitGameObjects, ray, scene->GetSelectedGameObject());
#endif
	AddIntersectionQuadtree(hitGameObjects, ray, scene->GetLoadedScene()->GetRootQuadtree());
	AddIntersectionDynamicObjects(hitGameObjects, ray, scene->GetLoadedScene()->GetNonStaticObjects());

	GetRaycastHitInfo(hitGameObjects, ray, hit, exceptionGameObject);

	if (hit.gameObject != nullptr)
	{
		return true;
	}

	return false;
}

bool Physics::RaycastFirst(const LineSegment& ray)
{
	std::map<float, const GameObject*> hitGameObjects;
	ModuleScene* scene = App->GetModule<ModuleScene>();

#ifdef ENGINE
	AddIntersectionGameObject(hitGameObjects, ray, scene->GetSelectedGameObject());
#endif
	if (hitGameObjects.size() == 0)
	{
		AddFirstFoundIntersectionQuadtree(hitGameObjects, ray, scene->GetLoadedScene()->GetRootQuadtree());
	}
	if (hitGameObjects.size() == 0)
	{
		AddFirstFoundIntersectionDynamicObjects(hitGameObjects, ray, scene->GetLoadedScene()->GetNonStaticObjects());
	}

	if (hitGameObjects.size() != 0)
	{
		return true;
	}

	return false;
}

bool Physics::HasIntersection(const LineSegment& ray, GameObject* go, float& nearDistance, float& farDistance)
{
	ComponentTransform* transform = static_cast<ComponentTransform*>(go->GetComponent(ComponentType::TRANSFORM));
	if (transform)
	{
		bool hit = ray.Intersects(transform->GetEncapsuledAABB(), nearDistance, farDistance);

		if (hit && go->IsActive())
		{
			return true;
		}
	}

	return false;
}

void Physics::AddIntersectionGameObject(std::map<float, const GameObject*>& hitGameObjects, 
	const LineSegment& ray, GameObject* go)
{
	float nearDistance, farDistance;

	if (HasIntersection(ray, go, nearDistance, farDistance)) 
	{
		hitGameObjects[nearDistance] = go;
	}
}

void Physics::AddIntersectionDynamicObjects(std::map<float, const GameObject*>& hitGameObjects,
	const LineSegment& ray, const std::vector<GameObject*>& dynamicObjects)
{
	float nearDistance, farDistance;
	for (GameObject* gameObject : dynamicObjects)
	{
		if (HasIntersection(ray, gameObject, nearDistance, farDistance))
		{
			hitGameObjects[nearDistance] = gameObject;
		}
	}
}

void Physics::AddIntersectionQuadtree(std::map<float, const GameObject*>& hitGameObjects,
	const LineSegment& ray, Quadtree* quadtree)
{
	std::queue<const Quadtree*> quadtreeQueue;
	quadtreeQueue.push(quadtree);

	while (!quadtreeQueue.empty())
	{
		const Quadtree* currentQuadtree = quadtreeQueue.front();
		quadtreeQueue.pop();

		if (!ray.Intersects(currentQuadtree->GetBoundingBox()))
		{
			continue;
		}

		std::set<GameObject*> quadtreeGameObjects = currentQuadtree->GetGameObjects();

		float nearDistance, farDistance;
		for (GameObject* gameObject : quadtreeGameObjects)
		{
			if (HasIntersection(ray, gameObject, nearDistance, farDistance))
			{
				hitGameObjects[nearDistance] = gameObject;
			}
		}

		if (currentQuadtree->IsLeaf())
		{ 
			continue;
		}
		else
		{
			quadtreeQueue.push(currentQuadtree->GetFrontRightNode());
			quadtreeQueue.push(currentQuadtree->GetFrontLeftNode());
			quadtreeQueue.push(currentQuadtree->GetBackRightNode());
			quadtreeQueue.push(currentQuadtree->GetBackLeftNode());

		}
	}
}

void Physics::GetRaycastHitInfo(const std::map<float, const GameObject*>& hitGameObjects,
	const LineSegment& ray, RaycastHit& hit)
{
	GameObject* newSelectedGameObject = nullptr;

	float thisDistance = 0.0f;
	float minCurrentDistance = inf;
	float3 exactHitPoint = float3::zero;
	float3 nearestHitPoint = float3::zero;
	float3 hitNormal = float3::zero;

	for (const std::pair<float, const GameObject*>& hitGameObject : hitGameObjects)
	{
		const GameObject* actualGameObject = hitGameObject.second;
		if (actualGameObject)
		{
			ComponentMeshRenderer* componentMeshRenderer = static_cast<ComponentMeshRenderer*>
				(actualGameObject->GetComponent(ComponentType::MESHRENDERER));

			if (!componentMeshRenderer) 
			{
				continue;
			}
			std::shared_ptr<ResourceMesh> goMeshAsShared = componentMeshRenderer->GetMesh();

			if (!goMeshAsShared)
			{
				continue;
			}

			const float4x4& gameObjectModelMatrix = static_cast<ComponentTransform*>
				(actualGameObject->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

			const std::vector<Triangle>& meshTriangles = goMeshAsShared->RetrieveTriangles(gameObjectModelMatrix);

			for (const Triangle& triangle : meshTriangles)
			{
				bool hit = ray.Intersects(triangle, &thisDistance, &exactHitPoint);

				if (!hit)
				{
					continue;
				}

				if (thisDistance >= minCurrentDistance)
				{
					continue;
				}

				// Only save a gameObject when any of its triangles is hit 
				// and it is the nearest triangle to the frustum
				newSelectedGameObject = const_cast<GameObject*>(actualGameObject);
				minCurrentDistance = thisDistance;
				hitNormal = triangle.NormalCCW();
				nearestHitPoint = exactHitPoint;
			}
		}
	}

	hit.gameObject = newSelectedGameObject;
	hit.distance = minCurrentDistance;
	hit.hitPoint = nearestHitPoint;
	hit.normal = hitNormal;
}

void Physics::GetRaycastHitInfo(const std::map<float, const GameObject*>& hitGameObjects,
	const LineSegment& ray, RaycastHit& hit, GameObject* exceptionGameObject)
{
	GameObject* newSelectedGameObject = nullptr;

	float thisDistance = 0.0f;
	float minCurrentDistance = inf;
	float3 exactHitPoint = float3::zero;
	float3 nearestHitPoint = float3::zero;
	float3 hitNormal = float3::zero;

	std::vector<GameObject*> children = exceptionGameObject->GetChildren();

	for (const std::pair<float, const GameObject*>& hitGameObject : hitGameObjects)
	{
		const GameObject* actualGameObject = hitGameObject.second;
		
		bool isInside = false;

		auto it = std::find(children.begin(), children.end(), actualGameObject);

		isInside = it != children.end();
		if (actualGameObject && actualGameObject != exceptionGameObject && !isInside)
		{
			ComponentMeshRenderer* componentMeshRenderer = static_cast<ComponentMeshRenderer*>
				(actualGameObject->GetComponent(ComponentType::MESHRENDERER));

			if (!componentMeshRenderer)
			{
				continue;
			}
			std::shared_ptr<ResourceMesh> goMeshAsShared = componentMeshRenderer->GetMesh();

			if (!goMeshAsShared)
			{
				continue;
			}

			const float4x4& gameObjectModelMatrix = static_cast<ComponentTransform*>
				(actualGameObject->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

			const std::vector<Triangle>& meshTriangles = goMeshAsShared->RetrieveTriangles(gameObjectModelMatrix);

			for (const Triangle& triangle : meshTriangles)
			{
				bool hit = ray.Intersects(triangle, &thisDistance, &exactHitPoint);

				if (!hit)
				{
					continue;
				}

				if (thisDistance >= minCurrentDistance)
				{
					continue;
				}

				// Only save a gameObject when any of its triangles is hit 
				// and it is the nearest triangle to the frustum
				newSelectedGameObject = const_cast<GameObject*>(actualGameObject);
				minCurrentDistance = thisDistance;
				hitNormal = triangle.NormalCCW();
				nearestHitPoint = exactHitPoint;
			}
		}
		else 
		{
			float chapuza = 0.0f;
		}
	}

	hit.gameObject = newSelectedGameObject;
	hit.distance = minCurrentDistance;
	hit.hitPoint = nearestHitPoint;
	hit.normal = hitNormal;
}

void Physics::AddFirstFoundIntersectionDynamicObjects(std::map<float, const GameObject*>& hitGameObjects,
	const LineSegment& ray, const std::vector<GameObject*>& dynamicObjects)
{
	float nearDistance, farDistance;
	for (GameObject* gameObject : dynamicObjects)
	{
		if (HasIntersection(ray, gameObject, nearDistance, farDistance))
		{
			hitGameObjects[nearDistance] = gameObject;
			return;
		}
	}
}

void Physics::AddFirstFoundIntersectionQuadtree(std::map<float, const GameObject*>& hitGameObjects,
	const LineSegment& ray, Quadtree* quadtree)
{
	std::queue<const Quadtree*> quadtreeQueue;
	quadtreeQueue.push(quadtree);

	while (!quadtreeQueue.empty())
	{
		const Quadtree* currentQuadtree = quadtreeQueue.front();
		quadtreeQueue.pop();

		if (!ray.Intersects(currentQuadtree->GetBoundingBox()))
		{
			continue;
		}

		if (currentQuadtree->IsLeaf())
		{
			std::set<GameObject*> quadtreeGameObjects = currentQuadtree->GetGameObjects();

			float nearDistance, farDistance;
			for (GameObject* gameObject : quadtreeGameObjects)
			{
				if (HasIntersection(ray, gameObject, nearDistance, farDistance))
				{
					hitGameObjects[nearDistance] = gameObject;
					return;
				}
			}
		}
		else 
		{
			quadtreeQueue.push(currentQuadtree->GetFrontRightNode());
			quadtreeQueue.push(currentQuadtree->GetFrontLeftNode());
			quadtreeQueue.push(currentQuadtree->GetBackRightNode());
			quadtreeQueue.push(currentQuadtree->GetBackLeftNode());

		}

	}
}