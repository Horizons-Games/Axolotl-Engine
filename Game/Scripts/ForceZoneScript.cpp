#include "StdAfx.h"
#include "ForceZoneScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

#ifdef ENGINE
#include "debugdraw.h"
#endif // ENGINE

REGISTERCLASS(ForceZoneScript);

ForceZoneScript::ForceZoneScript() : Script()
{
	REGISTER_FIELD(influenceRadius, float);
}

void ForceZoneScript::Start()
{
	position = GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalPosition();
}

void ForceZoneScript::Update(float deltaTime)
{
#ifdef ENGINE
	dd::sphere(position, dd::colors::Blue, influenceRadius);
#endif // ENGINE
}

float ForceZoneScript::GetInfluenceRadius() const
{
	return influenceRadius;
}
