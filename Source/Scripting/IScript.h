#pragma once
#include "IObject.h"

#include "Auxiliar/Reflection/Field.h"
#include <variant>

class GameObject;
class Application;

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

class IScript : public IObject
{
public:
	virtual ~IScript() override = default;

	virtual void Init() = 0;
	virtual void Start() = 0;
	virtual void PreUpdate(float deltaTime) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void PostUpdate(float deltaTime) = 0;
	virtual void CleanUp() = 0;

	virtual void SetGameObject(GameObject* owner) = 0;
	virtual void SetApplication(Application* app) = 0;

	const std::vector<ValidFieldType>& GetFields() const;

protected:
	GameObject* owner;
	Application* App;

private:
	std::vector<ValidFieldType> members;
};

inline const std::vector<ValidFieldType>& IScript::GetFields() const
{
	return members;
}