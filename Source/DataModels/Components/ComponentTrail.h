#pragma once
#include "Component.h"

#include "Auxiliar/Generics/Drawable.h"
#include "Auxiliar/Generics/Updatable.h"

class Trail;

class ComponentTrail : public Component, public Drawable, public Updatable
{
public:
	ComponentTrail(bool active, GameObject* owner);
	~ComponentTrail();

	void Draw() const override;
	void Update() override;
private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	Trail* trail;
};

