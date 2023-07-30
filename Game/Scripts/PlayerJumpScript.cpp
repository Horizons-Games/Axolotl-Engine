#include "StdAfx.h"
#include "PlayerJumpScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentScript.h"

#include "Auxiliar/Audio/AudioData.h"
#include "MathGeoLib/Include/Geometry/Ray.h"
#include "Physics/Physics.h"

#include "DebugDraw.h"

REGISTERCLASS(PlayerJumpScript);

void PlayerJumpScript::Start()
{

}


