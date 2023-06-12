#pragma once

#include "DataModels/GameObject/GameObject.h"
#include <ranges>

// The parameter name must be the exact name of the field inside the class
#define REGISTER_FIELD(name, Type) \
	this->members.push_back(axo::scripting::detail::fields::CreateField<Type>(#name, this->##name));

// The parameter Name must be one such that Get{Name} and Set{Name} functions exist as members of the class
#define REGISTER_FIELD_WITH_ACCESSORS(Name, Type)                                             \
	this->members.push_back(axo::scripting::detail::fields::CreateField<Type>( \
		#Name,                                                                                \
		[this]()                                                                              \
		{                                                                                     \
			return this->Get##Name();                                                         \
		},                                                                                    \
		[this](Type value)                                                                    \
		{                                                                                     \
			this->Set##Name(value);                                                           \
		}));

#define REGISTER_COMPONENT_WITH_ACCESSORS(Name, Type)                                                              \
	this->members.push_back(std::make_pair(FieldType::GAMEOBJECT,                                                  \
										   Field<GameObject*>(                                                     \
											   #Name,                                                              \
											   [this]                                                              \
											   {                                                                   \
												   Type* value = this->Get##Name();                                \
												   return value ? value->GetOwner() : nullptr;                     \
											   },                                                                  \
											   [this](GameObject* value)                                           \
											   {                                                                   \
												   this->Set##Name(value ? value->GetComponent<Type>() : nullptr); \
											   })));

// The parameter Name must be one such that Get{ Name } and Set{ Name } functions exist as members of the class
// And return a vector of the given type
#define REGISTER_VECTOR_WITH_ACCESSORS(Name, Type)                                                  \
	this->members.push_back(std::make_pair(FieldType::VECTOR,                                       \
										   VectorField(                                             \
											   #Name,                                               \
											   [this]                                               \
											   {                                                    \
												   using namespace axo::scripting::detail::vectors; \
												   return TypeToAny<Type>(this->Get##Name());       \
											   },                                                   \
											   [this](const std::vector<std::any>& value)           \
											   {                                                    \
												   using namespace axo::scripting::detail::vectors; \
												   this->Set##Name(AnyToType<Type>(value));         \
											   },                                                   \
											   TypeToEnum<Type>::value)));

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
// default overload, works for basic types
template<typename Type>
std::pair<FieldType, Field<Type>> CreateField(const std::string& name, Type& member)
{
	Field<Type> field(
		name,
		[&member]()
		{
			return member;
		},
		[&member](Type value)
		{
			member = value;
		});
	return std::make_pair(TypeToEnum<Type>::value, field);
}

// component overload, works for both Components and Scripts
template<typename Comp,
		 typename = std::enable_if_t<std::is_base_of<Component, Comp>::value || std::is_base_of<IScript, Comp>::value>>
std::pair<FieldType, Field<GameObject*>> CreateField(const std::string& name, Comp*& member)
{
	Field<GameObject*> field(
		name,
		[&member]()
		{
			return member ? member->GetOwner() : nullptr;
		},
		[&member](GameObject* value)
		{
			member = value ? value->GetComponent<Comp>() : nullptr;
		});
	return std::make_pair(FieldType::GAMEOBJECT, field);
}

// vector overload, works for vectors of basic types
template<typename Type>
std::pair<FieldType, VectorField> CreateField(const std::string& name, std::vector<Type>& member)
{
	VectorField field(
		name,
		[&member]()
		{
			using namespace axo::scripting::detail::vectors;
			return TypeToAny<Type>(member);
		},
		[&member](const std::vector<std::any>& value)
		{
			using namespace axo::scripting::detail::vectors;
			member = AnyToType<Type>(value);
		},
		TypeToEnum<Type>::value);
	std::make_pair(FieldType::VECTOR, field);
}

// vector of components overload, works for std::vector<Component> and std::vector<IScript>
template<typename Comp,
		 typename = std::enable_if_t<std::is_base_of<Component, Comp>::value || std::is_base_of<IScript, Comp>::value>>
std::pair<FieldType, VectorField> CreateField(const std::string& name, std::vector<Comp>& member)
{
	VectorField field(
		name,
		[&member]()
		{
			using namespace axo::scripting::detail::vectors;
			auto transformedVector =
				TypeToAny<Comp*>(member) | std::views::transform(
											   [](Comp* component)
											   {
												   return component ? component->GetOwner() : nullptr;
											   });
			return std::vector<GameObject*>(std::begin(transformedVector), std::end(transformedVector));
		},
		[&member](const std::vector<std::any>& value)
		{
			using namespace axo::scripting::detail::vectors;
			auto transformedVector =
				AnyToType<GameObject*>(value) | std::views::transform(
													[](GameObject* gameObject)
													{
														return gameObject ? gameObject->GetComponent<Comp>() : nullptr;
													});
			member = std::vector<Comp*>(std::begin(transformedVector), std::end(transformedVector));
		},
		FieldType::GAMEOBJECT);
	std::make_pair(FieldType::VECTOR, field);
}

// default overload, works for basic types
template<typename Type>
std::pair<FieldType, Field<Type>> CreateField(const std::string& name,
											  const std::function<Type(void)>& getter,
											  const std::function<void(Type)>& setter)
{
	Field<Type> field(
		name,
		[getter]()
		{
			return getter();
		},
		[setter](Type value)
		{
			setter(value);
		});
	return std::make_pair(TypeToEnum<Type>::value, field);
}
} // namespace fields
} // namespace axo::scripting::detail
