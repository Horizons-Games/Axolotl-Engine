#include "WindowComponentPlayer.h"

#include "Components/ComponentPlayer.h"

#include "Application.h"


WindowComponentPlayer::WindowComponentPlayer(ComponentPlayer* component) :
	ComponentWindow("PLAYER", component)
{
}

WindowComponentPlayer::~WindowComponentPlayer()
{
}

void WindowComponentPlayer::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
}
