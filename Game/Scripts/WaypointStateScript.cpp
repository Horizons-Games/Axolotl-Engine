#include "StdAfx.h"
#include "WaypointStateScript.h"

#include "Components/ComponentScript.h"

REGISTERCLASS(WaypointStateScript);

WaypointStateScript::WaypointStateScript() : Script(), waypointState(WaypointStates::AVAILABLE)
{
}

void WaypointStateScript::SetWaypointState(WaypointStates newState)
{
	waypointState = newState;
}

WaypointStates WaypointStateScript::GetWaypointState() const
{
	return waypointState;
}
