#pragma once
#include "Component.h"

class ComponentPlayer : public Component
{
public:
	ComponentPlayer(bool active, GameObject* owner);
	~ComponentPlayer() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	bool IsStatic();
	void SetStatic(bool newStatic);

	bool HaveMouseActivated();
	void SetMouse(bool newMouse);

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
