#pragma once
#include "Auxiliar/Generics/Drawable.h"
#include "DataModels/Components/Component.h"
class ComponentVideo : public Component, public Drawable
{
	ComponentVideo(bool active, GameObject* owner);
	~ComponentVideo() override;

	void Draw() const override;

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;
};