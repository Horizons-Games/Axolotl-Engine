#pragma once

#include "Module.h"
#include "Bullet/btBulletDynamicsCommon.h"

class ModulePhysics : public Module
{
public:
	ModulePhysics();
	~ModulePhysics() override;

	bool Init() override;

private:

	std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> dispatcher;
	std::unique_ptr<btBroadphaseInterface> overlappingPairCache;
	std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
	std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
};