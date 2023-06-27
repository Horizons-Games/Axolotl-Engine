#pragma once
#include "Auxiliar/Generics/Updatable.h"
#include "Component.h"

class ComponentObstacle: public Component, public Updatable
{
public:
	ComponentObstacle(bool active, GameObject* owner);
	~ComponentObstacle();

	void Update() override;

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;
};
