#pragma once
#include "Component.h"

class ComponentPlayer :
    public Component
{
public:
	ComponentPlayer(bool active, GameObject* owner);
	~ComponentPlayer() override;

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	bool IsStatic();
	void SetStatic(bool newStatic);
private:
	bool staticPlayer = false;
};

inline bool ComponentPlayer::IsStatic()
{
	return staticPlayer;
}

inline void ComponentPlayer::SetStatic(bool newStatic)
{
	staticPlayer = newStatic;
}

