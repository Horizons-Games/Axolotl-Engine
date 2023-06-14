#pragma once
#include "Component.h"

#pragma message(__FILE__ " TODO: Remove this component")

class ComponentPlayer : public Component
{
public:
	ComponentPlayer(bool active, GameObject* owner);
	~ComponentPlayer() override;

	bool IsStatic();
	void SetStatic(bool newStatic);

	bool HaveMouseActivated();
	void SetMouse(bool newMouse);

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(Json& meta) override;

private:
	bool staticPlayer = false;
	bool mousePlayer = false;
};

inline bool ComponentPlayer::IsStatic()
{
	return staticPlayer;
}

inline void ComponentPlayer::SetStatic(bool newStatic)
{
	staticPlayer = newStatic;
}

inline bool ComponentPlayer::HaveMouseActivated()
{
	return mousePlayer;
}
