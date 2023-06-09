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

	float GetImpulsionMul();
	void  SetImpulsionMul(float impulsion);
	void  SetSubscribed(bool subscribe);

	void BreakComponentBy(class ComponentRigidBody* other);
	void BreakComponent();
	bool GetSubscribed();

private:

	float impulsionForce = 1.0f;
	LCG lcg;
	btVector3 impulsion{ 1.0f,1.0f,1.0f };
	bool subscribed = true;
};

inline bool ComponentBreakable::GetSubscribed()
{
	return subscribed;
}

inline float ComponentBreakable::GetImpulsionMul()
{
	return impulsionForce;
}

inline void ComponentBreakable::SetImpulsionMul(float impulsion)
{
	impulsionForce = impulsion;
}

inline void ComponentBreakable::SetSubscribed(bool subscribe)
{
	subscribed = subscribe;
}


