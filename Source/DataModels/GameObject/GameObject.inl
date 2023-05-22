#pragma once

template<typename C>
C* GameObject::CreateComponent()
{
	return static_cast<C*>(CreateComponent(ComponentToEnum<C>::value));
}

template<typename C>
C* GameObject::GetComponent() const
{
	/*auto firstElement = std::ranges::find_if(components,
											 [](const std::unique_ptr<Component>& comp)
											 {
												 return compt->GetType() == ComponentToEnum<C>::value;
											 });
	if (firstElement == std::end(components))
	{
		return nullptr;
	}
	return static_cast<C*>(firstElement.get());*/
	return nullptr;
}

template<typename T>
const std::vector<T*> GameObject::GetComponentsByType(ComponentType type) const
{
	std::vector<T*> components;

	for (const std::unique_ptr<Component>& component : this->components)
	{
		if (component && component->GetType() == type)
		{
			components.push_back(dynamic_cast<T*>(component.get()));
		}
	}

	return components;
}
