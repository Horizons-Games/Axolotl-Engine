#pragma once
#include "GameObject.h"

template<typename C>
C* GameObject::CreateComponent()
{
	return static_cast<C*>(CreateComponent(ComponentToEnum<C>::value));
}

template<typename C>
C* GameObject::GetComponent() const
{
	auto firstElement = std::ranges::find_if(components,
											 [](const std::unique_ptr<Component>& comp)
											 {
												 return comp->GetType() == ComponentToEnum<C>::value;
											 });
	if (firstElement == std::end(components))
	{
		return nullptr;
	}
	return reinterpret_cast<C*>((*firstElement).get());
}

template<typename C>
std::vector<C*> GameObject::GetComponents() const
{
	auto filteredComponents = components |
							  std::views::filter(
								  [](const std::unique_ptr<Component>& comp)
								  {
									  return comp->GetType() == ComponentToEnum<C>::value;
								  }) |
							  std::views::transform(
								  [](const std::unique_ptr<Component>& comp)
								  {
									  return static_cast<C*>(comp.get());
								  });
	return std::vector<C*>(std::begin(filteredComponents), std::end(filteredComponents));
}

template<typename C>
inline bool GameObject::RemoveComponent()
{
	return RemoveComponent(GetComponent<C>());
}

template<typename C>
inline bool GameObject::RemoveComponents()
{
	auto removeIfResult = std::remove_if(std::begin(components),
										 std::end(components),
										 [](const std::unique_ptr<Component>& comp)
										 {
											 return comp->GetType() == ComponentToEnum<C>::value;
										 });
	components.erase(removeIfResult, std::end(components));
	return removeIfResult != std::end(components);
}
