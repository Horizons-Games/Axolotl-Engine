#pragma once
#include "../Component.h"
#include "Math/float3.h"

class ComponentButton :
	public Component
{
public:
	ComponentButton(bool active, GameObject* owner);
	~ComponentButton() override;

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void OnClicked();

	bool IsClicked() const;
	void SetClicked(bool clicked);

	float3& GetColorClicked();

private:
	bool clicked;
	float3 colorClicked;
};

inline bool ComponentButton::IsClicked() const
{
	return clicked;
}

inline void ComponentButton::SetClicked(bool clicked)
{
	this->clicked = clicked;
}

inline float3& ComponentButton::GetColorClicked()
{
	return colorClicked;
}