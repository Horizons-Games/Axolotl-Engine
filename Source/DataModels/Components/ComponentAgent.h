#pragma once
#include "Auxiliar/Generics/Updatable.h"
#include "Component.h"

class ComponentAgent: public Component, public Updatable
{
public:
	ComponentAgent(bool active, GameObject* owner);
	~ComponentAgent();

	void Update() override;

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

};
