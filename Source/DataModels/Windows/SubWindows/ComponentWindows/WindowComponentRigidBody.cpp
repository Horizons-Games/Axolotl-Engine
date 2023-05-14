#include "WindowComponentRigidBody.h"

#include "Components/ComponentRigidBody.h"
#include "Bullet/btBulletDynamicsCommon.h"

WindowComponentRigidBody::WindowComponentRigidBody(ComponentRigidBody* component) :
	ComponentWindow("RIGID BODY", component)
{
}

WindowComponentRigidBody::~WindowComponentRigidBody()
{
}

void WindowComponentRigidBody::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text(""); //used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent
	
	ComponentRigidBody* asRigidBody = static_cast<ComponentRigidBody*>(component);
    

	if (asRigidBody)
	{
        btRigidBody* rigidBody = asRigidBody -> GetRigidBody();
		ImGui::Text("");

		bool isKinematic = asRigidBody->GetIsKinematic();

        ImGui::Text("Is Kinematic"); ImGui::SameLine();
        if (ImGui::Checkbox("##Is Kinematic", &isKinematic))
        {
            asRigidBody->SetIsKinematic(isKinematic);
            asRigidBody->SetupMobility();
		}

		bool isStatic = asRigidBody->GetIsStatic();

        ImGui::Text("Is Static"); ImGui::SameLine();
        if (ImGui::Checkbox("##Is Static", &isStatic))
		{
			if (isStatic)
			{
                asRigidBody->SetIsKinematic(false);
			}
            asRigidBody->SetIsStatic(isStatic);
			asRigidBody->SetupMobility();
		}

        // Shape
        const char* shapeTypes[] = { "None", "Box", "Sphere"/*, "Capsule", "Cylinder", "Cone"*/ };

        int currentShape = asRigidBody->GetShape();
        if (ImGui::Combo("Shape", &currentShape, shapeTypes, IM_ARRAYSIZE(shapeTypes)))
        {
            asRigidBody->SetCollisionShape(static_cast<ComponentRigidBody::SHAPE>(currentShape));
        }
        if (currentShape > 0)
        {
            btVector3 minPoint, maxPoint;
            rigidBody->getAabb(minPoint, maxPoint);
            ImGui::Text("AABB: Min: %f, %f, %f", minPoint.x(), minPoint.y(), minPoint.z());
            ImGui::Text("AABB: Max: %f, %f, %f", maxPoint.x(), maxPoint.y(), maxPoint.z());
        }

        if (!isStatic)
        {
            // Mass
            float mass = rigidBody->getMass();
            if (ImGui::DragFloat("Mass", &mass, 0.1f, 0.0f, FLT_MAX))
            {
                btVector3 local_inertia;
                rigidBody->getCollisionShape()->calculateLocalInertia(mass, local_inertia);
                rigidBody->setMassProps(mass, local_inertia);
                asRigidBody->SetMass(mass);
            }
        }
        // Gravity
        btVector3 gravity = rigidBody->getGravity();
        if (ImGui::DragFloat3("Gravity", gravity.m_floats, 0.1f))
        {
            asRigidBody->SetGravity(gravity);
        }

        // Linear damping
        float linearDamping = rigidBody->getLinearDamping();
        if (ImGui::DragFloat("Linear Damping", &linearDamping, 0.01f, 0.0f, 1.0f))
        {
            //rigidBody->setDamping(linearDamping, rigidBody->getAngularDamping());
            asRigidBody->SetLinearDamping(linearDamping);
        }

        // Angular damping
        float angularDamping = rigidBody->getAngularDamping();
        if (ImGui::DragFloat("Angular Damping", &angularDamping, 0.01f, 0.0f, 1.0f))
        {
            //rigidBody->setDamping(rigidBody->getLinearDamping(), angularDamping);
            asRigidBody->SetAngularDamping(angularDamping);
        }

        // Restitution
        float restitution = rigidBody->getRestitution();
        if (ImGui::DragFloat("Restitution", &restitution, 0.01f, 0.0f, 1.0f))
        {
            //rigidBody->setRestitution(restitution);
            asRigidBody->SetRestitution(angularDamping);
        }

        float KpForce = asRigidBody->GetKpForce();
        float KpTorque = asRigidBody->GetKpTorque();
        bool isUsingPositionController = asRigidBody->GetUsePositionController();
        bool isUsingRotationController = asRigidBody->GetUseRotationController();

        ImGui::Text("Use Position Controller"); ImGui::SameLine();
        if (ImGui::Checkbox("##Use Position Controller", &isUsingPositionController))
        {
            asRigidBody->SetUsePositionController(isUsingPositionController);
        }

        if (isUsingPositionController)
        {
            if (ImGui::SliderFloat("KP Position", &KpForce, 0.5f, 10.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
            {
                asRigidBody->SetKpForce(KpForce);
            }
        }

        ImGui::Text("Use Rotation Controller"); ImGui::SameLine();
        if (ImGui::Checkbox("##Use Rotation Controller", &isUsingRotationController))
        {
            asRigidBody->SetUseRotationController(isUsingRotationController);
        }

        if (isUsingRotationController)
        {
            if (ImGui::SliderFloat("KP Rotation", &KpTorque, 0.05f, 2.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
            {
                asRigidBody->SetKpTorque(KpTorque);
            }
        }
	}
}
