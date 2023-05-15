#pragma once
#include "Components/Component.h"
#include "Auxiliar/Generics/Updatable.h"


class ComponentBreakable : public Component, public Updatable
{
public: 

	ComponentBreakable(bool active, GameObject* owner);
	~ComponentBreakable() override;

	void Update() override;

	void SuscribeToOnCollisionEnter();

	//TODO:Unsuscribe on destroy

	void OnCollisionEnter(class ComponentRigidBody* other);


private:

	bool suscribed = false;
};

