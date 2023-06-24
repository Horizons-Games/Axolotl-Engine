#pragma once
#include "Auxiliar/Generics/Updatable.h"
#include "DataModels/Components/Component.h"

#include "Math/float2.h"
#include <memory>

class ComponentSlider : public Component, public Updatable
{
public:
	ComponentSlider(bool active, GameObject* owner);
	~ComponentSlider() override;

	void Update() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	float GetMaxValue() const;
	float GetMinValue() const;
	float GetCurrentValue() const;
	float CalculateNormalizedValue() const;

	void SetMaxValue(float maxValue);
	void SetMinValue(float minValue);

	void SetBackground(GameObject* background);
	void SetFill(GameObject* fill);
	void SetHandle(GameObject* handle);

	void ModifyCurrentValue(float currentValue);

private:

	void OnHandleDragged();

	void SaveGameObject(Json& meta, const char* name, GameObject* go);
	GameObject* LoadGameObject(Json& meta, const char* name);
	GameObject* background;
	GameObject* fill;
	GameObject* handle;

	bool wasClicked;
	float2 actualClickHandlePosition;

	float maxValue = 100.0f;
	float minValue = 0.0f;

	float currentValue = 50.0f;
};

inline float ComponentSlider::GetMaxValue() const
{
	return maxValue;
}

inline float ComponentSlider::GetMinValue() const
{
	return minValue;
}

inline float ComponentSlider::GetCurrentValue() const
{
	return currentValue;
}

inline float ComponentSlider::CalculateNormalizedValue() const
{
	float normalizedValue = 0.0f;
	if (maxValue - minValue != 0)
	{
		normalizedValue = (currentValue - minValue) / (maxValue - minValue);
	};
	return normalizedValue;
}

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
