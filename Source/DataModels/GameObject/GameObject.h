#pragma once

#include <list>
#include <unordered_map>

#include "../../FileSystem/UniqueID.h"
#include <memory>
#include <iterator>

#include "Geometry/AABB.h"
#include "Geometry/OBB.h"

class Component;
class ComponentMeshRenderer;
class ComponentCanvas;
class Json;

enum class ComponentType;
enum class LightType;

enum class StateOfSelection
{
	NO_SELECTED,
	SELECTED,
	CHILD_SELECTED
};

class GameObject
{
public:
	explicit GameObject(const std::string& name);
	GameObject(const std::string& name, UID uid);
	GameObject(const std::string& name, GameObject* parent);
	GameObject(const GameObject& gameObject);
	~GameObject();

	void SaveOptions(Json& json);
	void LoadOptions(Json& meta);

	void Update();
	void Draw() const;
	void DrawSelected();
	void DrawHighlight();

	void InitNewEmptyGameObject(bool is3D=true);

	void AddChild(std::unique_ptr<GameObject> child);
	std::unique_ptr<GameObject> RemoveChild(const GameObject* child);

	UID GetUID() const;
	const char* GetName() const;
	const char* GetTag() const;
	GameObject* GetParent() const;

	StateOfSelection GetStateOfSelection() const;
	const std::vector<GameObject*> GetChildren() const;
	void SetChildren(std::vector<std::unique_ptr<GameObject>>& children);

	const std::vector<Component*> GetComponents() const;
	void SetComponents(std::vector<std::unique_ptr<Component>>& components);
	void CopyComponent(ComponentType type, Component* component);
	void CopyComponentLight(LightType type, Component* component);

	template <typename T,
		std::enable_if_t<std::is_base_of<Component, T>::value, bool> = true>
	const std::vector<T*> GetComponentsByType(ComponentType type) const;
	void SetStateOfSelection(StateOfSelection stateOfSelection);

	bool IsEnabled() const; // If the check for the GameObject is enabled in the Inspector
	void Enable();
	void Disable();

	void SetName(const char* newName);
	void SetTag(const char* newTag);
	void SetParent(GameObject* newParent);
	void MoveParent(GameObject* newParent);

	bool IsActive() const; // If it is active in the hierarchy (related to its parent/s)
	void DeactivateChildren();
	void ActivateChildren();

	Component* CreateComponent(ComponentType type);
	Component* CreateComponentLight(LightType lightType);
	Component* CreateComponentAudioSource();
	bool RemoveComponent(const Component* component);
	Component* GetComponent(ComponentType type) const;

	std::list<GameObject*> GetGameObjectsInside();

	void MoveUpChild(GameObject* childToMove);
	void MoveDownChild(GameObject* childToMove);

	void CalculateBoundingBoxes();
	void Encapsule(const vec* Vertices, unsigned numVertices);

	ComponentCanvas* FoundCanvasOnAnyParent();

	const AABB& GetLocalAABB();
	const AABB& GetEncapsuledAABB();
	const OBB& GetObjectOBB();
	const bool isDrawBoundingBoxes() const;

	void setDrawBoundingBoxes(bool newDraw);
	bool IsADescendant(const GameObject* descendant);
	void SetParentAsChildSelected();

	bool CompareTag(const std::string& commingTag) const;

private:
	bool IsAChild(const GameObject* child);

private:
	UID uid;
	UID parentUID;

	bool enabled;
	bool active;
	std::string name;
	std::string tag;
	std::vector<std::unique_ptr<Component>> components;
	StateOfSelection stateOfSelection;

	GameObject* parent;
	std::vector<std::unique_ptr<GameObject>> children;

	AABB localAABB;
	AABB encapsuledAABB;
	OBB objectOBB;
	bool drawBoundingBoxes;

	friend class WindowInspector;
};

inline UID GameObject::GetUID() const
{
	return uid;
}

inline void GameObject::SetStateOfSelection(StateOfSelection stateOfSelection)
{
	if (stateOfSelection == StateOfSelection::NO_SELECTED)
	{
		if (parent)
		{
			parent->SetStateOfSelection(StateOfSelection::NO_SELECTED);
		}
	}
	this->stateOfSelection = stateOfSelection;
}

inline bool GameObject::IsEnabled() const
{
	return enabled;
}

inline const char* GameObject::GetName() const
{
	return name.c_str();
}

inline void GameObject::SetName(const char* newName)
{
	name = newName;
}

inline void GameObject::SetParent(GameObject* newParent)
{
	parent = newParent;
}

inline GameObject* GameObject::GetParent() const
{
	return parent;
}

inline StateOfSelection GameObject::GetStateOfSelection() const
{
	return stateOfSelection;
}

inline const char* GameObject::GetTag() const
{
	return tag.c_str();
}

inline void GameObject::SetTag(const char* newTag)
{
	tag = newTag;
}

inline bool GameObject::IsActive() const
{
	return active;
}

inline const std::vector<GameObject*> GameObject::GetChildren() const
{
	std::vector<GameObject*> rawChildren;
	rawChildren.reserve(children.size());

	if(!children.empty())
		std::transform(std::begin(children), std::end(children), std::back_inserter(rawChildren), 
			[] (const std::unique_ptr<GameObject>& go) { return go.get(); });

	return rawChildren;
}

inline void GameObject::SetChildren(std::vector<std::unique_ptr<GameObject>>& children)
{
	this->children.clear();
	for (std::unique_ptr<GameObject>& newChild : children)
	{
		this->children.push_back(std::move(newChild));
	}
}

inline const std::vector<Component*> GameObject::GetComponents() const
{
	std::vector<Component*> rawComponent;

	std::transform(std::begin(components), std::end(components), std::back_inserter(rawComponent),
		[](const std::unique_ptr<Component>& c) { return c.get(); });

	return rawComponent;
}

template <typename T,
	std::enable_if_t<std::is_base_of<Component, T>::value, bool>>
inline const std::vector<T*> GameObject::GetComponentsByType(ComponentType type) const
{
	std::vector<T*> components;

	for (const std::unique_ptr<Component>& component : this->components)
	{
		if (component->GetType() == type)
		{
			components.push_back(dynamic_cast<T*>(component.get()));
		}
	}

	return components;
}

inline const AABB& GameObject::GetLocalAABB()
{
	CalculateBoundingBoxes();
	return localAABB;
}

inline const AABB& GameObject::GetEncapsuledAABB()
{
	CalculateBoundingBoxes();
	return encapsuledAABB;
}

inline const OBB& GameObject::GetObjectOBB()
{
	CalculateBoundingBoxes();
	return objectOBB;
}

inline const bool GameObject::isDrawBoundingBoxes() const
{
	return drawBoundingBoxes;
}

inline void GameObject::setDrawBoundingBoxes(bool newDraw)
{
	drawBoundingBoxes = newDraw;
}

inline bool GameObject::CompareTag(const std::string& commingTag) const
{
	return tag == commingTag;
}