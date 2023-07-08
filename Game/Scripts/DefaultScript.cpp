#include "DefaultScript.h"

REGISTERCLASS(DefaultScript);

DefaultScript::DefaultScript() : Script(), value(10), vecStr{"Esto", "es un", "vector string", "de prueba"},
vecFloat{ 1.2f,2.3f,3.3f }, fl3(2.1f, 1.2f, 1.4f), vecFloat3{ fl3, fl3 }, vecGO{(nullptr),(nullptr)}, vecBool{(true),(false)},
sentence("Horizons"), character(nullptr), check(true)
{
	REGISTER_FIELD(value, float);
	REGISTER_FIELD(sentence, std::string);
	REGISTER_FIELD_WITH_ACCESSORS(Float3, float3);
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
	ENGINE_LOG("%f %f %f ", fl3[2], fl3[1], fl3[0]);
	ENGINE_LOG("%s", sentence.c_str());

	if (character != nullptr)
	{
		ENGINE_LOG("%s", character->GetName().c_str())
	}

	ENGINE_LOG("%s", std::to_string(check).c_str());
}

//Float3
const float3& DefaultScript::GetFloat3() const
{
	return fl3;
}

void DefaultScript::SetFloat3(const float3& fl3)
{
	this->fl3 = fl3;
}

//Vectors
const std::vector<float>& DefaultScript::GetVectorFloat() const
{
	return vecFloat;
}

void DefaultScript::SetVectorFloat(const std::vector<float>& vecFloat)
{
	this->vecFloat = vecFloat;
}

const std::vector<float3>& DefaultScript::GetVectorFloat3() const
{
	return vecFloat3;
}

void DefaultScript::SetVectorFloat3(const std::vector<float3>& vecFloat3)
{
	this->vecFloat3 = vecFloat3;
}

const std::vector<std::string>& DefaultScript::GetVectorStr() const
{
	return vecStr;
}

void DefaultScript::SetVectorStr(const std::vector<std::string>& vecStr)
{
	this->vecStr = vecStr;
}

const std::vector<GameObject*>& DefaultScript::GetVectorGO() const
{
	return vecGO;
}

void DefaultScript::SetVectorGO(const std::vector<GameObject*>& vecGO)
{
	this->vecGO = vecGO;
}

const std::vector<bool>& DefaultScript::GetVectorBool() const
{
	return vecBool;
}

void DefaultScript::SetVectorBool(const std::vector<bool>& vecBool)
{
	this->vecBool = vecBool;
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
