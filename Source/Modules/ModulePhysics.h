#pragma once

#include "Module.h"
#include "Bullet/btBulletDynamicsCommon.h"
#include <unordered_map>
#include <unordered_set>

class btRigidBody;
struct btBroadphasePair;
class btCollisionDispatcher;
struct btDispatcherInfo;
class ComponentRigidBody;

class ModulePhysics : public Module
{
public:

	ModulePhysics();
	~ModulePhysics() override;

	bool Init() override;
	update_status PreUpdate() override;

	void ManageCollisions();

	void AddRigidBody(ComponentRigidBody* rb, btRigidBody* body);
	void RemoveRigidBody(ComponentRigidBody* rb, btRigidBody* body);

	uint64_t GetAmountOfRigidBodies() const { return rigidBodyComponents.size(); }

	bool CleanUp() override;

	void Reset();

private:

	std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> dispatcher;
	std::unique_ptr<btBroadphaseInterface> overlappingPairCache;
	std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
	std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;

	std::unordered_set<uint64_t> collisions;
	std::unordered_map<uint32_t, ComponentRigidBody*> rigidBodyComponents;
};