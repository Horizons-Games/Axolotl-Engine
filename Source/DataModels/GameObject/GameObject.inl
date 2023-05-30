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
bool GameObject::RemoveComponent()
{
	return RemoveComponent(GetComponent<C>());
}

template<typename C>
bool GameObject::RemoveComponents()
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

template<typename S, std::enable_if_t<std::is_base_of<IScript, S>::value, bool>>
S* GameObject::GetComponent()
{
	std::vector<ComponentScript*> componentScripts = GetComponents<ComponentScript>();
	auto componentWithScript = std::ranges::find_if(componentScripts,
													[](const ComponentScript* component)
													{
														return dynamic_cast<S*>(component->GetScript()) != nullptr;
													});
	return componentWithScript != std::end(componentScripts) ? dynamic_cast<S*>((*componentWithScript)->GetScript())
															 : nullptr;
}

template<typename S, std::enable_if_t<std::is_base_of<IScript, S>::value, bool>>
std::vector<S*> GameObject::GetComponents()
{
	std::vector<ComponentScript*> componentScripts = GetComponents<ComponentScript>();
	auto filteredScripts = componentScripts |
						   std::views::transform(
							   [](ComponentScript* component)
							   {
								   return dynamic_cast<S*>(component->GetScript());
							   }) |
						   std::views::filter(
							   [](S* script)
							   {
								   return script != nullptr;
							   });
	return std::vector<S*>(std::begin(filteredScripts), std::end(filteredScripts));
}
