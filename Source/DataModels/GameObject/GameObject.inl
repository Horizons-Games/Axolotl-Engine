#pragma once

#include "Auxiliar/ComponentNotFoundException.h"

#include <concepts>

namespace axo::detail
{
template<typename T>
concept IsScript = std::is_base_of<IScript, std::remove_pointer_t<T>>::value;

template<typename T>
concept CompleteType = requires
{
	sizeof(T);
};

#define ASSERT_TYPE_COMPLETE(T)                                                                    \
	static_assert(axo::detail::CompleteType<T>,                                                    \
				  "Trying to call method template with an incomplete type. Did you forget to add " \
				  "the corresponding include?");
} // namespace axo::detail

template<typename C>
C* GameObject::CreateComponent()
{
	ASSERT_TYPE_COMPLETE(C);
	return static_cast<C*>(CreateComponent(ComponentToEnum<C>::value));
}

template<typename C>
C* GameObject::GetComponentInternal() const
{
	ASSERT_TYPE_COMPLETE(C);
	if constexpr (axo::detail::IsScript<C>)
	{
		// GetComponents already makes sure the objects returned are not null
		std::vector<ComponentScript*> componentScripts = GetComponents<ComponentScript>();
		auto componentWithScript = std::ranges::find_if(componentScripts,
														[](const ComponentScript* component)
														{
															return dynamic_cast<C*>(component->GetScript()) != nullptr;
														});
		return componentWithScript != std::end(componentScripts) ? dynamic_cast<C*>((*componentWithScript)->GetScript())
																 : nullptr;
	}
	else
	{
		auto firstElement =
			std::ranges::find_if(components,
								 [](const std::unique_ptr<Component>& comp)
								 {
									 return comp != nullptr && comp->GetType() == ComponentToEnum<C>::value;
								 });
		return firstElement != std::end(components) ? static_cast<C*>((*firstElement).get()) : nullptr;
	}
}

template<typename C>
C* GameObject::GetComponent() const
{
	ASSERT_TYPE_COMPLETE(C);
	C* internalResult = GetComponentInternal<C>();
	if (internalResult == nullptr)
	{
		if constexpr (axo::detail::IsScript<C>)
		{
			throw ComponentNotFoundException("Script of type " + std::string(typeid(C).name()) + " not found");
		}
		else
		{
			throw ComponentNotFoundException("Component of type " + std::string(typeid(C).name()) + " not found");
		}
	}
	return internalResult;
}

template<typename C>
std::vector<C*> GameObject::GetComponents() const
{
	ASSERT_TYPE_COMPLETE(C);
	if constexpr (axo::detail::IsScript<C>)
	{
		// GetComponents already makes sure the objects returned are not null
		std::vector<ComponentScript*> componentScripts = GetComponents<ComponentScript>();
		auto filteredScripts = componentScripts |
							   std::views::transform(
								   [](ComponentScript* component)
								   {
									   return dynamic_cast<C*>(component->GetScript());
								   }) |
							   std::views::filter(
								   [](C* script)
								   {
									   return script != nullptr;
								   });
		return std::vector<C*>(std::begin(filteredScripts), std::end(filteredScripts));
	}
	else
	{
		auto filteredComponents = components |
								  std::views::filter(
									  [](const std::unique_ptr<Component>& comp)
									  {
										  return comp != nullptr && comp->GetType() == ComponentToEnum<C>::value;
									  }) |
								  std::views::transform(
									  [](const std::unique_ptr<Component>& comp)
									  {
										  return static_cast<C*>(comp.get());
									  });
		return std::vector<C*>(std::begin(filteredComponents), std::end(filteredComponents));
	}
}

template<typename C>
bool GameObject::RemoveComponent()
{
	ASSERT_TYPE_COMPLETE(C);
	return RemoveComponent(GetComponentInternal<C>());
}

template<typename C>
bool GameObject::RemoveComponents()
{
	ASSERT_TYPE_COMPLETE(C);
	auto removeIfResult = std::remove_if(std::begin(components),
										 std::end(components),
										 [](const std::unique_ptr<Component>& comp)
										 {
											 return comp == nullptr || comp->GetType() == ComponentToEnum<C>::value;
										 });
	components.erase(removeIfResult, std::end(components));
	return removeIfResult != std::end(components);
}

template<typename C>
inline bool GameObject::HasComponent() const
{
	ASSERT_TYPE_COMPLETE(C);
	return GetComponentInternal<C>() != nullptr;
}
