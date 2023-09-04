#pragma once
#include "Component.h"

class ComponentPlayer : public Component
{
public:
	ComponentPlayer(bool active, GameObject* owner);
	~ComponentPlayer() override;

	bool IsStatic() const;
	void SetStatic(bool newStatic);

	bool HaveMouseActivated();
	void SetMouse(bool newMouse);

	bool IsActualPlayer() const;
	void SetActualPlayer(bool actualPlayer, bool propagate = false);

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

private:
	bool staticPlayer = false;
	bool mousePlayer = false;
	bool actualPlayer = false;
};

inline bool ComponentPlayer::IsStatic() const
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

inline bool ComponentPlayer::IsActualPlayer() const
{
	return actualPlayer;
}