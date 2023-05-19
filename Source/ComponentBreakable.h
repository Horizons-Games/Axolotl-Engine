#pragma once
#include "Components/Component.h"
#include "Auxiliar/Generics/Updatable.h"
#include "Bullet/LinearMath/btVector3.h"

#include "MathGeoLib/Include/Algorithm/Random/LCG.h"


class ComponentBreakable : public Component, public Updatable
{
public: 

	ComponentBreakable(bool active, GameObject* owner);
	~ComponentBreakable() override;

	void Update() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void SubscribeToOnCollisionEnter();
	void UnsubscribeToOnCollisionEnter();
	//TODO:Unsuscribe on destroy

	void OnCollisionEnter(class ComponentRigidBody* other);
	bool GetSubscribed();

private:

	LCG* lcg = new LCG();
	btVector3 impulsion{ 2.0f,2.0f,2.0f };
	bool subscribed = false;
};

inline bool ComponentBreakable::GetSubscribed()
{
	return subscribed;
}

