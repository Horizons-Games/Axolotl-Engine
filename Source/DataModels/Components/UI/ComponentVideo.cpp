#include "StdAfx.h"

#include "ComponentVideo.h"


ComponentVideo::ComponentVideo(bool active, GameObject* owner) :
	Component(ComponentType::VIDEO, active, owner, true)
{
}

ComponentVideo::~ComponentVideo()
{
}

void ComponentVideo::Draw() const
{
}

void ComponentVideo::InternalSave(Json& meta)
{
}

void ComponentVideo::InternalLoad(const Json& meta)
{
}
