#pragma once
#include "Component.h"

#include "Math/float3.h"

class ComponentMockState :
	public Component
{
public:
	ComponentMockState(bool active, GameObject* owner);
	~ComponentMockState() override;

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	bool GetIsWinState();
	bool GetIsFailState();

	void SetIsWinState(bool isWinState);
	void SetIsFailState(bool isFailState);

private:

	bool isWinState;
	bool isFailState;
};

inline bool ComponentMockState::GetIsWinState()
{
	return this->isWinState;
}

inline bool ComponentMockState::GetIsFailState()
{
	return this->isFailState;
}

inline void ComponentMockState::SetIsWinState(bool isWinState)
{
	this->isWinState = isWinState;
}

inline void ComponentMockState::SetIsFailState(bool isFailState)
{
	this->isFailState = isFailState;
}

