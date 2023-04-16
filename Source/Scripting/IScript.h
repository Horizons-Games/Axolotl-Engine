#pragma once
#include "IObject.h"

#include "Auxiliar/Reflection/Field.h"
#include <variant>
#include <optional>

class GameObject;
class Application;

#define REGISTER_FIELD(Name, Type, TypeEnum) \
	this->members.push_back(std::make_pair(FieldType::TypeEnum, Field<Type>( \
		#Name, \
		[this] { return this->Get##Name(); }, \
		[this](const Type& value) { this->Set##Name(value); } \
	)));

//for now only allow floats
using ValidFieldType = std::variant<Field<float>>;
using TypeFieldPair = std::pair<FieldType, ValidFieldType>;

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

	const std::vector<TypeFieldPair>& GetFields() const;
	template<typename T>
	std::optional<Field<T>> GetField(const std::string& name) const;
	void SetFields(const std::vector<TypeFieldPair>& members);

protected:
	GameObject* owner;
	Application* App;
	std::vector<TypeFieldPair> members;
};

inline const std::vector<TypeFieldPair>& IScript::GetFields() const
{
	return members;
}

inline void IScript::SetFields(const std::vector<TypeFieldPair>& members)
{
	for (TypeFieldPair enumAndField : members)
	{
		this->members.push_back(enumAndField);
	}
}

template<typename T>
inline std::optional<Field<T>> IScript::GetField(const std::string& name) const
{
	for (const TypeFieldPair& enumAndType : members)
	{
		Field<T> field = std::get<Field<T>>(enumAndType.second);
		if (field.name == name)
		{
			return field;
		}
	}
	return std::nullopt;
}
