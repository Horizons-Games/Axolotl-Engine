#pragma once
#include "../Component.h"
#include "Math/float3.h"

class ComponentButton : public Component
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

	void SetColorClicked(float3 colorClicked);
	void SetColorHovered(float3 colorHovered);

	float3& GetColorClicked();
	float3& GetColorHovered();

	const char* GetSceneName() const;

private:
	bool clicked;
	bool hovered;
	float3 colorClicked;
	float3 colorHovered;

	std::string sceneName;
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

inline void ComponentButton::SetColorClicked(float3 colorClicked)
{
	this->colorClicked = colorClicked;
}

inline void ComponentButton::SetColorHovered(float3 colorHovered)
{
	this->colorHovered = colorHovered;
}

inline float3& ComponentButton::GetColorClicked()
{
	return colorClicked;
}

inline float3& ComponentButton::GetColorHovered()
{
	return colorHovered;
}

inline const char* ComponentButton::GetSceneName() const
{
	return sceneName.c_str();
}
