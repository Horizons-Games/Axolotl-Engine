#pragma once

#include "Component.h"
#include "Auxiliar/Reflection/Field.h"

#include <vector>
#include <variant>
#include <any>

#define REGISTER_FIELD(Name, Type, TypeEnum) \
	Field<Type> field( \
		#Name, \
		[this] { return this->Get##Name(); }, \
		[this](const Type& value) { this->Set##Name(value); }, \
		FieldType::##TypeEnum \
	); \
	this->members.push_back(field);

//for now only allow floats
using ValidFieldType = std::variant<Field<float>>;

class ComponentScript : public Component
{
public:
	ComponentScript(GameObject* owner);
	virtual ~ComponentScript() override;

	virtual void Init() override {};
	virtual void Start() {};
	virtual void PreUpdate() {};
	virtual void Update() override {};
	virtual void PostUpdate() {};
	virtual void CleanUp() {};

	const std::vector<ValidFieldType>& GetFields() const;

private:
	std::vector<ValidFieldType> members;
};

inline const std::vector<ValidFieldType>& ComponentScript::GetFields() const
{
	return members;
}
