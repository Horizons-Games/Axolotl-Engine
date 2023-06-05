#pragma once
#include "Components/Component.h"
#include "Auxiliar/Generics/Updatable.h"
#include<memory>

class GameObject;

class ComponentLine : public Component, public Updatable
{
public: 
	ComponentLine(bool active, GameObject* owner);
	~ComponentLine() override;

	void Update() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

private:
	GameObject* childGameObject;
};
