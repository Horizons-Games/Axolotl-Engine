#pragma once

#include "Component.h"
#include "Auxiliar/Reflection/Field.h"

#include <vector>
#include <variant>
#include <any>

#define REGISTER_FIELD(Name, Type) \
	this->RegisterField(#Name, [this] { return this->Get##name(); }, [this](std::any value) { this->Set##name(value); }), FieldType::##Type)

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

protected:
	void RegisterField(const std::string& name, const std::function<std::any(void)>& getter, const std::function<void(const std::any&)> setter, FieldType type);

private:
	std::vector<ValidFieldType> members;
};

inline const std::vector<ValidFieldType>& ComponentScript::GetFields() const
{
	return members;
}
