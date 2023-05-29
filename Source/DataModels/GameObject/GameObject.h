#pragma once

#include <functional>
#include <list>
#include <memory>
#include <ranges>

#include "../../FileSystem/UniqueID.h"
#include "MathGeoLib/Include/Math/vec2d.h"

class Component;
class ComponentMeshRenderer;
class ComponentCanvas;
class Json;
class ResourceModel;

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
	using GameObjectView =
		std::ranges::transform_view<std::ranges::ref_view<const std::vector<std::unique_ptr<GameObject>>>,
									std::function<GameObject*(const std::unique_ptr<GameObject>&)>>;
	using ComponentView =
		std::ranges::transform_view<std::ranges::ref_view<const std::vector<std::unique_ptr<Component>>>,
									std::function<Component*(const std::unique_ptr<Component>&)>>;

	explicit GameObject(const std::string& name);
	GameObject(const std::string& name, UID uid);
	GameObject(const std::string& name, GameObject* parent);
	GameObject(const GameObject& gameObject);
	~GameObject();

	void SaveOptions(Json& json);
	void LoadOptions(Json& meta);

	void Draw() const;

	void InitNewEmptyGameObject(bool is3D = true);

	void LinkChild(GameObject* child);
	[[nodiscard]] GameObject* UnlinkChild(const GameObject* child);

	UID GetUID() const;
	std::string GetName() const;
	std::string GetTag() const;
	GameObject* GetParent() const;
	GameObject* GetRootGO() const;

	StateOfSelection GetStateOfSelection() const;
	void SetStateOfSelection(StateOfSelection stateOfSelection);

	GameObjectView GetChildren() const;
	std::list<GameObject*> GetAllDescendants();
	void SetChildren(std::vector<std::unique_ptr<GameObject>>& children);

	void SetComponents(std::vector<std::unique_ptr<Component>>& components);
	void CopyComponent(Component* component);
	void CopyComponentLight(LightType type, Component* component);

	template<typename C>
	C* CreateComponent();
	template<typename C>
	C* GetComponent() const;
	ComponentView GetComponents() const;
	template<typename C>
	std::vector<C*> GetComponents() const;
	template<typename C>
	bool RemoveComponent();
	template<typename C>
	bool RemoveComponents();
	bool RemoveComponent(const Component* component);

	Component* CreateComponentLight(LightType lightType);

	bool IsEnabled() const; // If the check for the GameObject is enabled in the Inspector
	void Enable();
	void Disable();

	void SetName(const std::string& newName);
	void SetTag(const std::string& newTag);
	void SetParent(GameObject* newParent);
	void SetRootGO(GameObject* newRootGO);

	bool IsActive() const; // If it is active in the hierarchy (related to its parent/s)
	void DeactivateChildren();
	void ActivateChildren();

	bool IsStatic();
	void SetStatic(bool newStatic);
	void SpreadStatic();

	void MoveUpChild(const GameObject* childToMove);
	void MoveDownChild(const GameObject* childToMove);

	bool IsADescendant(const GameObject* descendant);

	bool CompareTag(const std::string& commingTag) const;

	GameObject* FindGameObject(const std::string& name);

private:
	GameObject(const std::string& name,
			   GameObject* parent,
			   UID uid,
			   bool enabled,
			   bool active,
			   StateOfSelection selection,
			   bool staticObject);

	Component* CreateComponent(ComponentType type);

	bool IsAChild(const GameObject* child);

	enum class HierarchyDirection
	{
		UP,
		DOWN
	};
	void MoveChild(const GameObject* child, HierarchyDirection direction);

private:
	UID uid;

	bool enabled;
	bool active;
	bool staticObject;
	std::string name;
	std::string tag;
	std::vector<std::unique_ptr<Component>> components;
	StateOfSelection stateOfSelection;

	GameObject* parent;
	GameObject* root;
	std::vector<std::unique_ptr<GameObject>> children;

	friend class WindowInspector;
};

inline UID GameObject::GetUID() const
{
	return uid;
}

inline void GameObject::SetStateOfSelection(StateOfSelection stateOfSelection)
{
	this->stateOfSelection = stateOfSelection;
	if (parent == nullptr)
	{
		return;
	}
	switch (stateOfSelection)
	{
	case StateOfSelection::NO_SELECTED:
		parent->SetStateOfSelection(StateOfSelection::NO_SELECTED);
		break;
	case StateOfSelection::SELECTED:
	case StateOfSelection::CHILD_SELECTED:
		parent->SetStateOfSelection(StateOfSelection::CHILD_SELECTED);
		break;
	}
}

inline bool GameObject::IsEnabled() const
{
	return enabled;
}

inline std::string GameObject::GetName() const
{
	return name;
}

inline void GameObject::SetName(const std::string& newName)
{
	name = newName;
}

inline void GameObject::SetRootGO(GameObject* newRootGO)
{
	root = newRootGO;
}

inline GameObject* GameObject::GetParent() const
{
	return parent;
}

inline GameObject* GameObject::GetRootGO() const
{
	return root;
}

inline StateOfSelection GameObject::GetStateOfSelection() const
{
	return stateOfSelection;
}

inline std::string GameObject::GetTag() const
{
	return tag;
}

inline void GameObject::SetTag(const std::string& newTag)
{
	tag = newTag;
}

inline bool GameObject::IsActive() const
{
	return active;
}

inline GameObject::GameObjectView GameObject::GetChildren() const
{
	// I haven't found a way allow for an anonymous function
	std::function<GameObject*(const std::unique_ptr<GameObject>&)> lambda = [](const std::unique_ptr<GameObject>& go)
	{
		return go.get();
	};
	return std::ranges::transform_view(children, lambda);
}

inline void GameObject::SetChildren(std::vector<std::unique_ptr<GameObject>>& children)
{
	this->children.clear();
	for (std::unique_ptr<GameObject>& newChild : children)
	{
		this->children.push_back(std::move(newChild));
	}
}

inline GameObject::ComponentView GameObject::GetComponents() const
{
	// I haven't found a way allow for an anonymous function
	std::function<Component*(const std::unique_ptr<Component>&)> lambda = [](const std::unique_ptr<Component>& c)
	{
		return c.get();
	};
	return std::ranges::transform_view(components, lambda);
}

inline bool GameObject::CompareTag(const std::string& commingTag) const
{
	return tag == commingTag;
}

inline bool GameObject::IsStatic()
{
	return staticObject;
}

inline void GameObject::SetStatic(bool newStatic)
{
	staticObject = newStatic;
}

#include "DataModels/GameObject/GameObject.inl"
