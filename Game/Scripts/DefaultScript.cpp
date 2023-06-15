#include "DefaultScript.h"

REGISTERCLASS(DefaultScript);

DefaultScript::DefaultScript() : Script(), value(10), vecStr{"Esto", "es un", "vector string", "de prueba"},
vec{ 1.2f,2.3f,3.3f }, vec3(2.1f, 1.2f, 1.4f), vecVec3{ vec3, vec3 }, vecGO{(nullptr),(nullptr)}, vecBool{(true),(false)},
sentence("Horizons"), character(nullptr), check(true)
{
	REGISTER_FIELD(value, float);
	REGISTER_FIELD(sentence, std::string);
	REGISTER_FIELD_WITH_ACCESSORS(Vector3, float3);
	REGISTER_VECTOR_WITH_ACCESSORS(VectorFloat, float);
	REGISTER_VECTOR_WITH_ACCESSORS(VectorFloat3, float3);
	REGISTER_VECTOR_WITH_ACCESSORS(VectorStr, std::string);
	REGISTER_VECTOR_WITH_ACCESSORS(VectorGO, GameObject*);
	REGISTER_VECTOR_WITH_ACCESSORS(VectorBool, bool);
	REGISTER_FIELD_WITH_ACCESSORS(Character, GameObject*);
	REGISTER_FIELD(check, bool);
}

void DefaultScript::Update(float deltaTime)
{
	ENGINE_LOG("%f", value);
	ENGINE_LOG("%f %f %f ", vec3[2], vec3[1], vec3[0]);
	ENGINE_LOG("%s", sentence.c_str());

	if (character != nullptr)
	{
		ENGINE_LOG("%s", character->GetName().c_str())
	}

	ENGINE_LOG("%s", std::to_string(check).c_str());
}

//FLOAT3
const float3& DefaultScript::GetVector3() const
{
	return vec3;
}

void DefaultScript::SetVector3(const float3& vec3)
{
	this->vec3 = vec3;
}

//VECTORS
const std::vector<float>& DefaultScript::GetVectorFloat() const
{
	return vec;
}

void DefaultScript::SetVectorFloat(const std::vector<float>& vec)
{
	this->vec = vec;
}

const std::vector<float3>& DefaultScript::GetVectorFloat3() const
{
	return vecVec3;
}

void DefaultScript::SetVectorFloat3(const std::vector<float3>& vec)
{
	this->vecVec3 = vec;
}

const std::vector<std::string>& DefaultScript::GetVectorStr() const
{
	return vecStr;
}

void DefaultScript::SetVectorStr(const std::vector<std::string>& vec)
{
	this->vecStr = vec;
}

const std::vector<GameObject*>& DefaultScript::GetVectorGO() const
{
	return vecGO;
}

void DefaultScript::SetVectorGO(const std::vector<GameObject*>& vec)
{
	this->vecGO = vec;
}

const std::vector<bool>& DefaultScript::GetVectorBool() const
{
	return vecBool;
}

void DefaultScript::SetVectorBool(const std::vector<bool>& vec)
{
	this->vecBool = vec;
}

//GameObject
GameObject* DefaultScript::GetCharacter() const
{
	return character;
}

void DefaultScript::SetCharacter(GameObject* character)
{
	ENGINE_LOG("My Character has been changed!");
	this->character = character;
}
