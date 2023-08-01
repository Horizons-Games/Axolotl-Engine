#pragma once

#include "DataModels/GameObject/GameObject.h"
#include <concepts>
#include <ranges>

// The parameter name must be the exact name of the field inside the class
#define REGISTER_FIELD(name, Type)                                                    \
	this->AddMember(axo::scripting::detail::CreateField(#name,                        \
														std::function<Type(void)>(    \
															[this]()                  \
															{                         \
																return this->##name;  \
															}),                       \
														std::function<void(Type)>(    \
															[this](Type value)        \
															{                         \
																this->##name = value; \
															})));

// The parameter Name must be one such that Get{Name} and Set{Name} functions exist as members of the class
#define REGISTER_FIELD_WITH_ACCESSORS(Name, Type)                                   \
	this->AddMember(axo::scripting::detail::CreateField(#Name,                      \
														std::function<Type(void)>(  \
															[this]()                \
															{                       \
																return Get##Name(); \
															}),                     \
														std::function<void(Type)>(  \
															[this](Type value)      \
															{                       \
																Set##Name(value);   \
															})));

namespace axo::scripting::detail
{
// Ideally, all of this should be changed to use std::ranges::views

template<typename Type>
std::vector<std::any> TypeToAny(const std::vector<Type>& vector)
{
	std::vector<std::any> convertedVector;
	convertedVector.reserve(vector.size());
	for (const Type& element : vector)
	{
		convertedVector.push_back(element);
	}
	return convertedVector;
}

template<typename Type>
std::vector<Type> AnyToType(const std::vector<std::any>& vector)
{
	std::vector<Type> convertedVector;
	convertedVector.reserve(vector.size());
	for (const std::any& element : vector)
	{
		convertedVector.push_back(std::any_cast<Type>(element));
	}
	return convertedVector;
}

template<typename T>
concept IsComponentOrScript = std::is_base_of<Component, std::remove_pointer_t<T>>::value ||
	std::is_base_of<IScript, std::remove_pointer_t<T>>::value;

// vector overload
template<typename Type>
TypeFieldPair CreateField(std::string&& name,
						  std::function<std::vector<Type>(void)>&& getter,
						  std::function<void(std::vector<Type>)>&& setter)
{
	if constexpr (IsComponentOrScript<Type>)
	{
		VectorField field(
			std::move(name),
			[getter]()
			{
				auto transformedVector = getter() | std::views::transform(
														[](Type component)
														{
															return component ? component->GetOwner() : nullptr;
														});
				return TypeToAny<GameObject*>(
					std::vector<GameObject*>(std::begin(transformedVector), std::end(transformedVector)));
			},
			[setter](const std::vector<std::any>& value)
			{
				auto transformedVector =
					AnyToType<GameObject*>(value) |
					std::views::transform(
						[](GameObject* gameObject)
						{
							return gameObject ? gameObject->GetComponent<std::remove_pointer_t<Type>>() : nullptr;
						});
				setter(std::vector<Type>(std::begin(transformedVector), std::end(transformedVector)));
			},
			FieldType::GAMEOBJECT);
		return std::make_pair(FieldType::VECTOR, field);
	}
	else
	{
		VectorField field(
			std::move(name),
			[getter]()
			{
				return TypeToAny<Type>(getter());
			},
			[setter](const std::vector<std::any>& value)
			{
				setter(AnyToType<Type>(value));
			},
			TypeToEnum<Type>::value);
		return std::make_pair(FieldType::VECTOR, field);
	}
}

// default overload, works for non-vector types
template<typename Type>
TypeFieldPair CreateField(std::string&& name, std::function<Type(void)>&& getter, std::function<void(Type)>&& setter)
{
	if constexpr (IsComponentOrScript<Type>)
	{
		Field<GameObject*> field(
			std::move(name),
			[getter]()
			{
				Type member = getter();
				return member ? member->GetOwner() : nullptr;
			},
			[setter](GameObject* value)
			{
				setter(value ? value->GetComponent<std::remove_pointer_t<Type>>() : nullptr);
			});
		return std::make_pair(FieldType::GAMEOBJECT, field);
	}
	else
	{
		Field<Type> field(std::move(name), std::move(getter), std::move(setter));
		return std::make_pair(TypeToEnum<Type>::value, field);
	}
}

} // namespace axo::scripting::detail
