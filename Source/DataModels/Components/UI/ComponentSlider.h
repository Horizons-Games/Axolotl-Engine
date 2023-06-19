#pragma once
#include "Auxiliar/Generics/Updatable.h"
#include "DataModels/Components/Component.h"

#include <memory>

class ComponentSlider : public Component, public Updatable
{
public:
	ComponentSlider(bool active, GameObject* owner);
	~ComponentSlider() override;

	void Update() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void SetBackground(GameObject* background);
	void SetFill(GameObject* fill);
	void SetHandle(GameObject* handle);

private:
	GameObject* background;
	GameObject* fill;
	GameObject* handle;

	float maxValue = 100.0f;
	float minValue = 0.0f;

	float currentValue = 50.0f;
	float normalizedValue = 0.5f;
};

inline void ComponentSlider::SetBackground(GameObject* background)
{
	this->background = background;
}

inline void ComponentSlider::SetFill(GameObject* fill)
{
	this->fill = fill;
}

inline void ComponentSlider::SetHandle(GameObject* handle)
{
	this->handle = handle;
}