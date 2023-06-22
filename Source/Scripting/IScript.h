#pragma once
#include "IObject.h"

#include "RuntimeObjectSystem/ISimpleSerializer.h"

#include "Auxiliar/Reflection/Field.h"
#include "Auxiliar/Reflection/VectorField.h"
#include "Enums/FieldType.h"

#include "Math/float3.h"

#include <optional>
#include <variant>

class GameObject;
class Application;
class ComponentRigidBody;

using ValidFieldType =
	std::variant<Field<float>, Field<float3>, VectorField, Field<std::string>, Field<GameObject*>, Field<bool>>;
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
	virtual void OnCollisionEnter(ComponentRigidBody* other) = 0;
	virtual void OnCollisionExit(ComponentRigidBody* other) = 0;
	virtual void CleanUp() = 0;

	virtual GameObject* GetOwner() const = 0;
	virtual void SetOwner(GameObject* owner) = 0;

	virtual const std::vector<TypeFieldPair>& GetFields() const = 0;
};
