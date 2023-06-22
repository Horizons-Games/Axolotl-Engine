#pragma once

#include "DataModels/GameObject/GameObject.h"
#include <ranges>

// The parameter name must be the exact name of the field inside the class
#define REGISTER_FIELD(name, Type)                                     \
	this->AddMember(axo::scripting::detail::fields::CreateField<Type>( \
		#name,                                                         \
		[this]()                                                       \
		{                                                              \
			return this->##name;                                       \
		},                                                             \
		[this](Type value)                                             \
		{                                                              \
			this->##name = value;                                      \
		}));

// The parameter Name must be one such that Get{Name} and Set{Name} functions exist as members of the class
#define REGISTER_FIELD_WITH_ACCESSORS(Name, Type)                      \
	this->AddMember(axo::scripting::detail::fields::CreateField<Type>( \
		#Name,                                                         \
		[this]()                                                       \
		{                                                              \
			return Get##Name();                                        \
		},                                                             \
		[this](Type value)                                             \
		{                                                              \
			Set##Name(value);                                          \
		}));

namespace axo::scripting::detail
{
// Ideally, all of this should be changed to use std::ranges::views
namespace vectors
{
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
} // namespace vectors
namespace fields
{
template<typename>
struct IsVector : std::false_type
{
};

template<typename T, typename Allocator>
struct IsVector<std::vector<T, Allocator>> : std::true_type
{
};

// default overload, works for basic types
template<typename Type,
		 typename = std::enable_if_t<!IsVector<Type>::value &&
									 !std::is_base_of<Component, std::remove_pointer_t<Type>>::value &&
									 !std::is_base_of<IScript, std::remove_pointer_t<Type>>::value>>
std::pair<FieldType, Field<Type>> CreateField(const std::string& name,
											  const std::function<Type(void)>& getter,
											  const std::function<void(Type)>& setter)
{
	Field<Type> field(name, getter, setter);
	return std::make_pair(TypeToEnum<Type>::value, field);
}

// component overload, works for both Components and Scripts
template<typename Comp,
		 typename = std::enable_if_t<std::is_base_of<Component, std::remove_pointer_t<Comp>>::value ||
									 std::is_base_of<IScript, std::remove_pointer_t<Comp>>::value>>
std::pair<FieldType, Field<GameObject*>> CreateField(const std::string& name,
													 const std::function<Comp(void)>& getter,
													 const std::function<void(Comp)>& setter)
{
	Field<GameObject*> field(
		name,
		[getter]()
		{
			Comp member = getter();
			return member ? member->GetOwner() : nullptr;
		},
		[setter](GameObject* value)
		{
			setter(value ? value->GetComponent<std::remove_pointer_t<Comp>>() : nullptr);
		});
	return std::make_pair(FieldType::GAMEOBJECT, field);
}
// vector overload, works for vectors of basic types
template<
	typename Vec,
	typename = std::enable_if_t<IsVector<Vec>::value &&
								!std::is_base_of<Component, std::remove_pointer_t<typename Vec::value_type>>::value &&
								!std::is_base_of<IScript, std::remove_pointer_t<typename Vec::value_type>>::value>>
std::pair<FieldType, VectorField>
	CreateField(const std::string& name, const std::function<Vec(void)>& getter, const std::function<void(Vec)>& setter)
{
	using Type = typename Vec::value_type;
	VectorField field(
		name,
		[getter]()
		{
			using namespace axo::scripting::detail::vectors;
			return TypeToAny<Type>(getter());
		},
		[setter](const std::vector<std::any>& value)
		{
			using namespace axo::scripting::detail::vectors;
			setter(AnyToType<Type>(value));
		},
		TypeToEnum<Type>::value);
	return std::make_pair(FieldType::VECTOR, field);
}

// vector of components overload, works for std::vector<Component> and std::vector<IScript>
// template<
//	typename Vec,
//	typename = std::enable_if_t<IsVector<Vec>::value &&
//								(std::is_base_of<Component, std::remove_pointer_t<typename Vec::value_type>>::value ||
//								 std::is_base_of<IScript, std::remove_pointer_t<typename Vec::value_type>>::value)>>
// std::pair<FieldType, VectorField>
//	CreateField(const std::string& name, const std::function<Vec(void)>& getter, const std::function<void(Vec)>& setter)
//{
//	using Comp = typename Vec::value_type;
//	VectorField field(
//		name,
//		[getter]()
//		{
//			using namespace axo::scripting::detail::vectors;
//			auto transformedVector =
//				TypeToAny<Comp*>(getter()) | std::views::transform(
//												 [](Comp* component)
//												 {
//													 return component ? component->GetOwner() : nullptr;
//												 });
//			return std::vector<GameObject*>(std::begin(transformedVector), std::end(transformedVector));
//		},
//		[setter](const std::vector<std::any>& value)
//		{
//			using namespace axo::scripting::detail::vectors;
//			auto transformedVector =
//				AnyToType<GameObject*>(value) | std::views::transform(
//													[](GameObject* gameObject)
//													{
//														return gameObject ? gameObject->GetComponent<Comp>() : nullptr;
//													});
//			setter(std::vector<Comp*>(std::begin(transformedVector), std::end(transformedVector)));
//		},
//		FieldType::GAMEOBJECT);
//	return std::make_pair(FieldType::VECTOR, field);
//}
} // namespace fields
} // namespace axo::scripting::detail
