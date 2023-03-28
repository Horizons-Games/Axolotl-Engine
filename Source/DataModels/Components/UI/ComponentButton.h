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
	bool IsHovered() const;
	void SetClicked(bool clicked);
	void SetHovered(bool hovered);

	float3& GetColorClicked();
	float3& GetColorHovered();

private:
	bool clicked;
	bool hovered;
	float3 colorClicked;
	float3 colorHovered;
};

inline bool ComponentButton::IsClicked() const
{
	return clicked;
}

inline bool ComponentButton::IsHovered() const
{
	return hovered;
}

inline void ComponentButton::SetClicked(bool clicked)
{
	this->clicked = clicked;
}

inline void ComponentButton::SetHovered(bool hovered)
{
	this->hovered = hovered;
}

inline float3& ComponentButton::GetColorClicked()
{
	return colorClicked;
}

inline float3& ComponentButton::GetColorHovered()
{
	return colorHovered;
}
