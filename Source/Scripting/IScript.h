#pragma once
#include "IObject.h"

#include "RuntimeObjectSystem/ISimpleSerializer.h"

#include "Auxiliar/Reflection/Field.h"
#include "Auxiliar/Reflection/VectorField.h"
#include "Enums/FieldType.h"

#include "Math/float3.h"

#include <variant>
#include <optional>

class GameObject;
class Application;

#include "Scripting/RegisterFieldMacros.h"

using ValidFieldType = std::variant<Field<float>, Field<float3>, VectorField, Field<std::string>, Field<GameObject*>, Field<bool>>;
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
	App = app;
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
		if (TypeToEnum<T>::value == enumAndType.first)
		{
			Field<T> field = std::get<Field<T>>(enumAndType.second);
			if (field.name == name)
			{
				return field;
			}
		}
	}
	return std::nullopt;
}

template<>
inline std::optional<Field<std::vector<std::any>>> IScript::GetField(const std::string& name) const
{
	for (const TypeFieldPair& enumAndType : members)
	{
		if (FieldType::VECTOR== enumAndType.first)
		{
			VectorField field = std::get<VectorField>(enumAndType.second);
			if (field.name == name)
			{
				return field;
			}
		}
	}
	return std::nullopt;
}

inline void IScript::Serialize(ISimpleSerializer* pSerializer)
{
	SERIALIZE(owner);
	SERIALIZE(App);
	for (const TypeFieldPair& enumAndField : members)
	{
		switch (enumAndField.first)
		{
		case FieldType::FLOAT:
		{
			Field<float> field = std::get<Field<float>>(enumAndField.second);
			float value = field.getter();
			pSerializer->SerializeProperty(field.name.c_str(), value);
			field.setter(value);
			break;
		}

		case FieldType::FLOAT3:
		{
			Field<float3> field = std::get<Field<float3>>(enumAndField.second);
			float3 value = field.getter();
			pSerializer->SerializeProperty(field.name.c_str(), value);
			field.setter(value);
			break;
		}

		case FieldType::VECTOR:
		{
			VectorField field = std::get<VectorField>(enumAndField.second);
			std::vector<std::any> value = field.getter();
			pSerializer->SerializeProperty(field.name.c_str(), value);
			field.setter(value);
			break;
		}

		case FieldType::STRING:
		{
			Field<std::string> field = std::get<Field<std::string>>(enumAndField.second);
			std::string value = field.getter();
			pSerializer->SerializeProperty(field.name.c_str(), value);
			field.setter(value);
			break;
		}

		case FieldType::GAMEOBJECT:
		{
			Field<GameObject*> field = std::get<Field<GameObject*>>(enumAndField.second);
			GameObject* value = field.getter();
			pSerializer->SerializeProperty(field.name.c_str(), value);
			field.setter(value);
			break;
		}

		case FieldType::BOOLEAN:
		{
			Field<bool> field = std::get<Field<bool>>(enumAndField.second);
			bool value = field.getter();
			pSerializer->SerializeProperty(field.name.c_str(), value);
			field.setter(value);
			break;
		}

		default:
			break;
		}
	}
}
