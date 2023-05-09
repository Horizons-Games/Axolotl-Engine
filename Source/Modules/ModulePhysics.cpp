#include "ModulePhysics.h"

#include "Bullet/btBulletDynamicsCommon.h"
#include "Application.h"

#include "GameObject/GameObject.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"

ModulePhysics::ModulePhysics()
{
}

ModulePhysics::~ModulePhysics()
{
}

void NearCallback(btBroadphasePair& collisionPair, btCollisionDispatcher& dispatcher, btDispatcherInfo& dispatchInfo)
{
	// Do your collision logic here, here you can filter which objects collide with each other if you have multiple collision layers
	// Only dispatch the Bullet collision information if you want the physics to continue    
	dispatcher.defaultNearCallback(collisionPair, dispatcher, dispatchInfo);
}

bool ModulePhysics::Init()
{
	//collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache = std::make_unique<btDbvtBroadphase>();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = std::make_unique<btSequentialImpulseConstraintSolver>();
	
	dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(dispatcher.get(), overlappingPairCache.get(), solver.get(), collisionConfiguration.get());

	dynamicsWorld->setGravity(btVector3(0, -10, 0));

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
    if(App->IsOnPlayMode())
    {
        dynamicsWorld->stepSimulation(App->GetDeltaTime(), 10);
        ManageCollisions();
    }
    
    return update_status::UPDATE_CONTINUE;
}


void ModulePhysics::ManageCollisions()
{

    struct ContactResultCallback : public btCollisionWorld::ContactResultCallback
    {
        bool collisionDetected;
        const btCollisionObject* otherRigidBody;

        ContactResultCallback() : collisionDetected(false), otherRigidBody(nullptr) {}

        virtual btScalar addSingleResult(btManifoldPoint& cp,
            const btCollisionObjectWrapper* colObj0Wrap,
            int partId0,
            int index0,
            const btCollisionObjectWrapper* colObj1Wrap,
            int partId1,
            int index1)
        {
            collisionDetected = true;
            otherRigidBody = colObj1Wrap->getCollisionObject();
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
                    ComponentRigidBody* other = static_cast<ComponentRigidBody*>(result.otherRigidBody->getUserPointer());
                    assert(rb && other);
                    uint64_t i1 = rb->GetID();
                    i1 = i1 << 32;
                    uint64_t i2 = other->GetID();
                    //key is the combination of the two indexes in the high 32 bits store the first index and in the low 32 bits store the second index
                    uint64_t key = i1 | i2;
                    if (collisions.find(key) == collisions.end())
                    {
                        rb->OnCollisionEnter(other);
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

    //iterate over collision and remove the ones which does not have contact
    for (auto it = collisions.begin(); it != collisions.end();)
    {
        uint32_t i1 = *it >> 32;
        uint32_t i2 = *it & 0x00000000FFFFFFFF; //remove high 32 bits
        ComponentRigidBody* rb = static_cast<ComponentRigidBody*>(rigidBodyComponents[i1]);
        ComponentRigidBody* other = static_cast<ComponentRigidBody*>(rigidBodyComponents[i2]);
        if (!rb || !other)
        {
            it = collisions.erase(it);
            continue;
        }

        //check if the collision still exists
        ContactResultCallback result;
        dynamicsWorld->contactPairTest(rb->GetRigidBody(), other->GetRigidBody(), result);
        if (!result.collisionDetected)
        {
            rb->OnCollisionExit(other);
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