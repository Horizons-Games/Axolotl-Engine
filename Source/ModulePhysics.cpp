#include "ModulePhysics.h"
#include "Bullet/btBulletDynamicsCommon.h"

ModulePhysics::ModulePhysics()
{
}

ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Init()
{
    return true;
}

update_status ModulePhysics::Update()
{
    return update_status::UPDATE_CONTINUE;
}


bool ModulePhysics::CleanUp()
{
    return true;
}