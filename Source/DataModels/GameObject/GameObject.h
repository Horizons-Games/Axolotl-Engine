#pragma once

#include "FileSystem/UID.h"
#include "MathGeoLib/Include/Math/vec2d.h"

#include <ranges>

class Component;
class ComponentMeshRenderer;
class ComponentCanvas;
class ComponentScript;
class Json;
class ResourceModel;
class IScript;

enum class ComponentType;
enum class LightType;
enum class AreaType;

enum class StateOfSelection
{
	NOT_SELECTED,
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

	void Save(Json& json);
	void Load(const Json& meta);

	void LoadComponents(const Json& jsonComponents);

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

	ComponentView GetComponents() const;
	void SetComponents(std::vector<std::unique_ptr<Component>>& components);
	void CopyComponent(Component* component);
	void CopyComponentLight(LightType type, Component* component);

	template<typename C>
	C* CreateComponent();
	// This method is intended to be used by the classes of the Engine, not its users
	// In case the component of the given type is not found, a nullptr is returned
	template<typename C>
	C* GetComponentInternal() const;
	// This method is intended to be used by the users of the Engine
	// In case the component of the given type is not found, a ComponentNotFoundException is thrown
	template<typename C>
	C* GetComponent() const;
	template<typename C>
	std::vector<C*> GetComponents() const;
	template<typename C>
	bool RemoveComponent();
	template<typename C>
	bool RemoveComponents();
	bool RemoveComponent(const Component* component);

	// This method is intended to be used by the classes of the Engine, not its users
	// In case the script of the given type is not found, a nullptr is returned
	template<typename S, std::enable_if_t<std::is_base_of<IScript, S>::value, bool> = true>
	S* GetComponentInternal();
	// This method is intended to be used by the users of the Engine
	// In case the script of the given type is not found, a ComponentNotFoundException is thrown
	template<typename S, std::enable_if_t<std::is_base_of<IScript, S>::value, bool> = true>
	S* GetComponent();
	template<typename S, std::enable_if_t<std::is_base_of<IScript, S>::value, bool> = true>
	std::vector<S*> GetComponents();

	Component* CreateComponentLight(LightType lightType, AreaType areaType);

	// This method returns true if and only if the GameObject is currently enabled
	bool IsEnabled() const;
	void Enable();
	void Disable();

	// This method returns true if IsEnabled returns true for this GameObject and for all its "ancestors" in the
	// hierarchy
	bool IsActive() const;

	void SetName(const std::string& newName);
	void SetTag(const std::string& newTag);
	void SetParent(GameObject* newParent);
	void SetRootGO(GameObject* newRootGO);

	bool IsStatic() const;
	void SetStatic(bool newStatic);
	void SpreadStatic();

	void MoveUpChild(const GameObject* childToMove);
	void MoveDownChild(const GameObject* childToMove);

	bool IsADescendant(const GameObject* descendant);
	bool IsRendereable();

	bool CompareTag(const std::string& commingTag) const;

	GameObject* FindGameObject(const std::string& name);

private:
	GameObject(const std::string& name,
			   GameObject* parent,
			   UID uid,
			   bool enabled,
			   bool active,
			   StateOfSelection selection,
			   bool staticObject,
			   const std::string& tag = std::string());

	Component* CreateComponent(ComponentType type);

	void Activate();
	void Deactivate();
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
		case StateOfSelection::NOT_SELECTED:
			parent->SetStateOfSelection(StateOfSelection::NOT_SELECTED);
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
	return active && enabled;
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

inline bool GameObject::IsStatic() const
{
	return staticObject;
}

#include "DataModels/GameObject/GameObject.inl"
