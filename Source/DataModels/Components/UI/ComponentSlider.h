#pragma once
#include "DataModels/Components/Component.h"

#include "Math/float2.h"
#include <memory>

enum class DirectionSlider
{
	LEFT_TO_RIGHT,
	RIGHT_TO_LEFT,
	DOWN_TO_TOP,
	TOP_TO_DOWN,
	CIRCLE_RIGHT,
	CIRCLE_LEFT,
	CIRCLE_TOP,
	CIRCLE_DOWN
};

class ComponentSlider : public Component
{
public:
	ComponentSlider(bool active, GameObject* owner);
	~ComponentSlider() override;

	void CheckSlider();

	float GetMaxValue() const;
	float GetMinValue() const;
	float GetCurrentValue() const;
	float CalculateNormalizedValue() const;
	GameObject* GetBackground() const;
	GameObject* GetFill() const;
	GameObject* GetHandle() const;
	DirectionSlider GetDirection() const;

	void SetMaxValue(float maxValue);
	void SetMinValue(float minValue);
	void SetBackground(GameObject* background);
	void SetFill(GameObject* fill);
	void SetHandle(GameObject* handle);
	void SetDirection(int direction);

	void ModifyCurrentValue(float currentValue);
	void OnHandleDragged();

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	void SaveGameObject(Json& meta, const char* name, GameObject* go);
	GameObject* LoadGameObject(const Json& meta, const char* name);
	GameObject* background;
	GameObject* fill;
	GameObject* handle;

	float maxValue = 100.0f;
	float minValue = 0.0f;

	float currentValue = 50.0f;
	DirectionSlider direction;
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

inline GameObject* ComponentSlider::GetBackground() const
{
	return background;
}

inline GameObject* ComponentSlider::GetFill() const
{
	return fill;
}

inline GameObject* ComponentSlider::GetHandle() const
{
	return handle;
}

inline DirectionSlider ComponentSlider::GetDirection() const
{
	return direction;
}

inline void ComponentSlider::SetBackground(GameObject* background)
{
	this->background = background;
}

inline void ComponentSlider::SetHandle(GameObject* handle)
{
	this->handle = handle;
}
