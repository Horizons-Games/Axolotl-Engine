#pragma once

#include "Bullet/LinearMath/btIDebugDraw.h"
#include "Bullet/btBulletDynamicsCommon.h"
#include "Module.h"
#include <unordered_map>
#include <unordered_set>

class btRigidBody;
struct btBroadphasePair;
class btCollisionDispatcher;
struct btDispatcherInfo;
class ComponentRigidBody;

class GLDebugDrawer : public btIDebugDraw
{
	int debugMode;

public:
	virtual void
		drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

	virtual void drawSphere(const btVector3& p, btScalar radius, const btVector3& color);

	virtual void drawTriangle(
		const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha);

	virtual void drawContactPoint(
		const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

	virtual void drawAabb(const btVector3& from, const btVector3& to, const btVector3& color);

	virtual void reportErrorWarning(const char* warningString);

	virtual void draw3dText(const btVector3& location, const char* textString);

	virtual void setDebugMode(int newDebugMode)
	{
		debugMode = newDebugMode;
	}

	virtual int getDebugMode() const
	{
		return debugMode;
	}
};

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

	uint64_t GetAmountOfRigidBodies() const
	{
		return rigidBodyComponents.size();
	}

	bool CleanUp() override;

	void Reset();

	bool GetDrawWorld() const;
	void SetDrawWorld(bool newDrawWorld);

	void UpdateDrawableRigidBodies(int value);

private:
	std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> dispatcher;
	std::unique_ptr<btBroadphaseInterface> overlappingPairCache;
	std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
	std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;

	GLDebugDrawer DebugDrawer;

	std::unordered_set<uint64_t> collisions;
	std::unordered_map<uint32_t, ComponentRigidBody*> rigidBodyComponents;

	bool drawWorld = false;
	int drawableRigidBodies = 0;
};

inline bool ModulePhysics::GetDrawWorld() const
{
	return drawWorld;
}

inline void ModulePhysics::SetDrawWorld(bool newDrawWorld)
{
	drawWorld = newDrawWorld;
}