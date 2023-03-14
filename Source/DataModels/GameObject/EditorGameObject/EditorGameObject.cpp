#include "EditorGameObject.h"

#include <queue>

#include "Application.h"
#include "Modules/ModuleDebugDraw.h"

#include "DataModels/Components/Component.h"
#include "DataModels/Components/DrawableComponent.h"
#include "DataModels/Components/ComponentMeshRenderer.h"

EditorGameObject::EditorGameObject(const char* name) : GameObject(name)
{
}

EditorGameObject::EditorGameObject(const char* name, GameObject* parent) : GameObject(name, parent)
{
}

EditorGameObject::~EditorGameObject()
{
}

void EditorGameObject::DrawBoundingBoxes()
{
	if (this->IsDrawBoundingBoxes())
	{
		App->debug->DrawBoundingBox(this->GetObjectOBB());
	}
}

void EditorGameObject::DrawSelected()
{
	std::queue<GameObject*> gameObjectQueue;
	gameObjectQueue.push(this);
	while (!gameObjectQueue.empty())
	{
		GameObject* currentGo = gameObjectQueue.front();
		gameObjectQueue.pop();
		for (GameObject* child : currentGo->GetChildren())
		{
			if (child->IsEnabled())
			{
				gameObjectQueue.push(child);
			}
		}
		std::vector<Component*> currentGameObjectComponents = currentGo->GetComponents();
		for (Component* component : currentGameObjectComponents)
		{
			if (component->IsDrawable() && component->IsActive())
			{
				static_cast<DrawableComponent*>(component)->Draw();
			}
		}
		if (currentGo->IsDrawBoundingBoxes())
		{
			App->debug->DrawBoundingBox(currentGo->GetObjectOBB());
		}
	}
}

void EditorGameObject::DrawHighlight()
{
	std::queue<const GameObject*> gameObjectQueue;
	gameObjectQueue.push(this);
	while (!gameObjectQueue.empty())
	{
		const GameObject* currentGo = gameObjectQueue.front();
		gameObjectQueue.pop();
		for (GameObject* child : currentGo->GetChildren())
		{
			if (child->IsEnabled())
			{
				gameObjectQueue.push(child);
			}
		}
		std::vector<ComponentMeshRenderer*> meshes =
			currentGo->GetComponentsByType<ComponentMeshRenderer>(ComponentType::MESHRENDERER);
		for (ComponentMeshRenderer* mesh : meshes)
		{
			mesh->DrawHighlight();
		}
	}
}
