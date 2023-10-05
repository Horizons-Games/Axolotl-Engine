#pragma once
#include "../Component.h"
#include "Math/float4.h"

class ConnectedCallback;

class ComponentButton : public Component
{
public:
	ComponentButton(bool active, GameObject* owner);
	~ComponentButton() override;

	void OnClicked();

	bool IsClicked() const;
	bool IsHovered() const;
	void SetClicked(bool clicked);
	void SetHovered(bool hovered);

	void SetColorClicked(const float4& colorClicked);
	void SetColorHovered(const float4& colorHovered);

	float4 GetColorClicked() const;
	float4 GetColorHovered() const;

	const char* GetSceneName() const;

	std::unique_ptr<ConnectedCallback> SetOnClickedCallback(std::function<void(void)>&& callback);

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	void SignalDisable() override;

private:
	bool clicked;
	bool hovered;
	float4 colorClicked;
	float4 colorHovered;

	std::string sceneName;
	std::optional<std::function<void(void)>> onClickedCallback;
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

inline void ComponentButton::SetColorClicked(const float4& colorClicked)
{
	this->colorClicked = colorClicked;
}

inline void ComponentButton::SetColorHovered(const float4& colorHovered)
{
	this->colorHovered = colorHovered;
}

inline float4 ComponentButton::GetColorClicked() const
{
	return colorClicked;
}

inline float4 ComponentButton::GetColorHovered() const
{
	return colorHovered;
}

inline const char* ComponentButton::GetSceneName() const
{
	return sceneName.c_str();
}
