#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

enum class WaypointStates
{
	AVAILABLE,
	UNAVAILABLE // For when the waypoint is covered by another object, it becomes unreacheable
};

class WaypointStateScript : public Script
{
public:
	WaypointStateScript();
	~WaypointStateScript() override = default;

	void SetWaypointState(WaypointStates newState);
	WaypointStates GetWaypointState() const;

private:
	WaypointStates waypointState;
};