#pragma once

#include "Component.h"
#include "Auxiliar/Reflection/Field.h"

#include <vector>

//for now only allow floats
using ValidFieldType = Field<float>;

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
	void SetFields(const std::vector<ValidFieldType>& members);
	void RegisterField(const ValidFieldType& field);

private:
	std::vector<ValidFieldType> members;
};

inline const std::vector<ValidFieldType>& ComponentScript::GetFields() const
{
	return members;
}

inline void ComponentScript::SetFields(const std::vector<ValidFieldType>& members)
{
	this->members = members;
}

inline void ComponentScript::RegisterField(const ValidFieldType& field)
{
	members.push_back(field);
}
