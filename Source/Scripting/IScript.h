#pragma once
#include "IObject.h"

#include "RuntimeObjectSystem/ISimpleSerializer.h"

#include "Auxiliar/Reflection/Field.h"
#include "Auxiliar/Reflection/TypeToEnum.h"
#include <variant>
#include <optional>

class GameObject;
class Application;

#define REGISTER_FIELD(Name, Type) \
	this->members.push_back(std::make_pair(TypeToEnum<Type>::value, Field<Type>( \
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

	void SetGameObject(GameObject* owner);
	void SetApplication(Application* app);

	const std::vector<TypeFieldPair>& GetFields() const;
	template<typename T>
	std::optional<Field<T>> GetField(const std::string& name) const;

	void Serialize(ISimpleSerializer* pSerializer) override;

protected:
	GameObject* owner;
	Application* App;
	std::vector<TypeFieldPair> members;
};

inline void IScript::SetGameObject(GameObject* owner)
{
	this->owner = owner;
}

inline void IScript::SetApplication(Application* app)
{
	this->App = app;
}

inline const std::vector<TypeFieldPair>& IScript::GetFields() const
{
	return members;
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

inline void IScript::Serialize(ISimpleSerializer* pSerializer)
{
	SERIALIZE(owner);
	SERIALIZE(App);
	SERIALIZE(members);
}
