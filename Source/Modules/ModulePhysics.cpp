#include "ModulePhysics.h"

#include "Application.h"
#include "Bullet/btBulletDynamicsCommon.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "GameObject/GameObject.h"
#include "debugdraw.h"

#include "AxoLog.h"

#ifndef ENGINE
	#include "Modules/ModuleEditor.h"
	#include "Windows/WindowDebug.h"
#endif

#include <vector>

ModulePhysics::ModulePhysics()
{
}

ModulePhysics::~ModulePhysics()
{
}

void NearCallback(btBroadphasePair& collisionPair, btCollisionDispatcher& dispatcher, btDispatcherInfo& dispatchInfo)
{
	// Do your collision logic here, here you can filter which objects collide with each other if you have multiple
	// collision layers Only dispatch the Bullet collision information if you want the physics to continue
	dispatcher.defaultNearCallback(collisionPair, dispatcher, dispatchInfo);
}

bool ModulePhysics::Init()
{
	// collision configuration contains default setup for memory, collision setup. Advanced users can create their own
	// configuration.
	collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();

	/// use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see
	/// Extras/BulletMultiThreaded)
	dispatcher = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());

	/// btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache = std::make_unique<btDbvtBroadphase>();

	/// the default constraint solver. For parallel processing you can use a different solver (see
	/// Extras/BulletMultiThreaded)
	solver = std::make_unique<btSequentialImpulseConstraintSolver>();

	dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(
		dispatcher.get(), overlappingPairCache.get(), solver.get(), collisionConfiguration.get());

	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	dynamicsWorld->setDebugDrawer(&DebugDrawer);
	dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawContactPoints |
												  btIDebugDraw::DBG_DrawWireframe);

	return true;
}

bool ModulePhysics::CleanUp()
{
	collisions.clear();
	rigidBodyComponents.clear();

	return true;
}

void ModulePhysics::Reset()
{
	CleanUp();
	Init();
}

update_status ModulePhysics::PreUpdate()
{
#ifdef ENGINE
	if (App->IsOnPlayMode())
	{
		dynamicsWorld->stepSimulation(App->GetDeltaTime());
		ManageCollisions();
	}

	if (drawableRigidBodies > 0)
	{
		dynamicsWorld->debugDrawWorld();
	}

#else
    dynamicsWorld->stepSimulation(App->GetDeltaTime());
    ManageCollisions();

	if (App->GetModule<ModuleEditor>()->GetDebugOptions()->GetDrawPhysics())
	{
		dynamicsWorld->debugDrawWorld();
	}
#endif

	return update_status::UPDATE_CONTINUE;
}

void ModulePhysics::ManageCollisions()
{
	struct ContactResultCallback : public btCollisionWorld::ContactResultCallback
	{
		bool collisionDetected;
		std::vector<const btCollisionObject*> othersRigidBody;

		ContactResultCallback() : collisionDetected(false)
		{
		}

		virtual btScalar addSingleResult(btManifoldPoint& cp,
										 const btCollisionObjectWrapper* colObj0Wrap,
										 int partId0,
										 int index0,
										 const btCollisionObjectWrapper* colObj1Wrap,
										 int partId1,
										 int index1)
		{
			collisionDetected = true;
			othersRigidBody.push_back(colObj1Wrap->getCollisionObject());
			return 0;
		}
	};

	btCollisionObjectArray collisionArray = dynamicsWorld->getCollisionObjectArray();
	for (int i = 0; i < collisionArray.size(); i++)
	{
		btCollisionObject* obj = collisionArray[i];

		ContactResultCallback result;
		dynamicsWorld->contactTest(obj, result);

		if (result.collisionDetected)
		{
			if (obj->getUserPointer() != nullptr)
			{
				ComponentRigidBody* rb = static_cast<ComponentRigidBody*>(obj->getUserPointer());
				if (rb != nullptr)
				{
					for (int j = 0; j < result.othersRigidBody.size(); j++)
					{
						ComponentRigidBody* other =
							static_cast<ComponentRigidBody*>(result.othersRigidBody[j]->getUserPointer());
						assert(rb && other);
						uint64_t i1 = rb->GetID();
						i1 = i1 << 32;
						uint64_t i2 = other->GetID();
						// key is the combination of the two indexes in the high 32 bits store the first index and in
						// the low 32 bits store the second index
						uint64_t key = i1 | i2;
						if (collisions.find(key) == collisions.end())
						{
							rb->OnCollisionEnter(other);
							other->OnCollisionEnter(rb);
						}
						else
						{
							rb->OnCollisionStay(other);
						}
						collisions.insert(key);
					}
				}
			}
		}
	}

	// iterate over collision and remove the ones which does not have contact
	for (auto it = collisions.begin(); it != collisions.end();)
	{
		uint32_t i1 = *it >> 32;
		uint32_t i2 = *it & 0x00000000FFFFFFFF; // remove high 32 bits
		ComponentRigidBody* rb = static_cast<ComponentRigidBody*>(rigidBodyComponents[i1]);
		ComponentRigidBody* other = static_cast<ComponentRigidBody*>(rigidBodyComponents[i2]);
		if (!rb || !other)
		{
			it = collisions.erase(it);
			continue;
		}

		// check if the collision still exists
		ContactResultCallback result;
		dynamicsWorld->contactPairTest(rb->GetRigidBody(), other->GetRigidBody(), result);
		if (!result.collisionDetected)
		{
			rb->OnCollisionExit(other);
			other->OnCollisionExit(rb);
			it = collisions.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void ModulePhysics::AddRigidBody(ComponentRigidBody* rb, btRigidBody* body)
{
	dynamicsWorld->addRigidBody(body);
	rigidBodyComponents[rb->GetID()] = rb;
}

void ModulePhysics::RemoveRigidBody(ComponentRigidBody* rb, btRigidBody* body)
{
	if (dynamicsWorld)
	{
		dynamicsWorld->removeCollisionObject(body);
		dynamicsWorld->removeRigidBody(body);
	}
	rigidBodyComponents.erase(rb->GetID());
}

void ModulePhysics::UpdateDrawableRigidBodies(int value)
{
	drawableRigidBodies += value;
}

void GLDebugDrawer::drawLine(const btVector3& from,
							 const btVector3& to,
							 const btVector3& fromColor,
							 const btVector3& toColor)
{
	dd::line(
		reinterpret_cast<ddVec3_In>(from), reinterpret_cast<ddVec3_In>(to), reinterpret_cast<ddVec3_In>(fromColor));
}

void GLDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	dd::line(reinterpret_cast<ddVec3_In>(from), reinterpret_cast<ddVec3_In>(to), reinterpret_cast<ddVec3_In>(color));
}

void GLDebugDrawer::drawSphere(const btVector3& p, btScalar radius, const btVector3& color)
{
	dd::sphere(reinterpret_cast<ddVec3_In>(p), reinterpret_cast<ddVec3_In>(color), radius);
}

void GLDebugDrawer::drawTriangle(
	const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha)
{
	LOG_WARNING("drawTriangle not implemented");
}

void GLDebugDrawer::drawContactPoint(
	const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	dd::sphere(reinterpret_cast<ddVec3_In>(PointOnB), reinterpret_cast<ddVec3_In>(color), 0.1f, 5000);
}

void GLDebugDrawer::drawAabb(const btVector3& from, const btVector3& to, const btVector3& color)
{
	dd::aabb(reinterpret_cast<ddVec3_In>(from), reinterpret_cast<ddVec3_In>(to), reinterpret_cast<ddVec3_In>(color));
}

void GLDebugDrawer::reportErrorWarning(const char* warningString)
{
	LOG_WARNING(warningString);
}

void GLDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	dd::screenText(textString, reinterpret_cast<ddVec3_In>(location), dd::colors::White, 1, 100);
}