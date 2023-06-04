#include "WindowComponentRigidBody.h"

#include "Bullet/btBulletDynamicsCommon.h"
#include "Components/ComponentRigidBody.h"

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

	ImGui::Text(""); // used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent

	ComponentRigidBody* asRigidBody = static_cast<ComponentRigidBody*>(component);

	if (asRigidBody)
	{
		btRigidBody* rigidBody = asRigidBody->GetRigidBody();
		ImGui::Text("");
		ImGui::Text("RigidBody variables");
		ImGui::Separator();

		bool isKinematic = asRigidBody->GetIsKinematic();
		
		if (ImGui::Checkbox("##Is Kinematic", &isKinematic))
		{
			asRigidBody->SetIsKinematic(isKinematic);
			asRigidBody->SetUpMobility();
		}
		ImGui::SameLine();
		ImGui::Text("Is Kinematic");

		bool isStatic = asRigidBody->IsStatic();

		if (ImGui::Checkbox("##Is Static", &isStatic))
		{
			if (isStatic)
			{
				asRigidBody->SetIsKinematic(false);
			}
			asRigidBody->SetIsStatic(isStatic);
			asRigidBody->SetUpMobility();
		}
		ImGui::SameLine();
		ImGui::Text("Is Static");

		bool isTrigger = asRigidBody->IsTrigger();

		if (ImGui::Checkbox("##Is a Trigger", &isTrigger))
		{
			asRigidBody->SetIsTrigger(isTrigger);
		}
		ImGui::SameLine();
		ImGui::Text("Is a Trigger");
		ImGui::Text("");

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
			asRigidBody->SetLinearDamping(linearDamping);
		}

		// Angular damping
		float angularDamping = rigidBody->getAngularDamping();
		if (ImGui::DragFloat("Angular Damping", &angularDamping, 0.01f, 0.0f, 1.0f))
		{
			asRigidBody->SetAngularDamping(angularDamping);
		}

		// Restitution
		float restitution = rigidBody->getRestitution();
		if (ImGui::DragFloat("Restitution", &restitution, 0.01f, 0.0f, 1.0f))
		{
			asRigidBody->SetRestitution(angularDamping);
		}

		ImGui::Text("");
		ImGui::Text("Collider variables");
		ImGui::Separator();

		bool drawCollider = asRigidBody->GetDrawCollider();

		if (ImGui::Checkbox("##Draw collider", &drawCollider))
		{
			asRigidBody->SetDrawCollider(drawCollider);
		}
		ImGui::SameLine();
		ImGui::Text("Draw collider");

		// Shape
		const char* shapeTypes[] = { "None", "Box", "Sphere", "Capsule", "Cone" /*, "Cylinder", */ };

		int currentShape = static_cast<int>(asRigidBody->GetShape());
		if (ImGui::Combo("Shape", &currentShape, shapeTypes, IM_ARRAYSIZE(shapeTypes)))
		{
			asRigidBody->SetCollisionShape(static_cast<ComponentRigidBody::Shape>(currentShape));
		}

		ImGui::Text("");

		// Translate RigidBody
        if (currentShape > 0)
        {
            btVector3 btPosition = asRigidBody->GetRigidBodyOrigin();
            float xValue = btPosition.getX();
            float yValue = btPosition.getY();
            float zValue = btPosition.getZ();

			bool dirty = false;

			ImGui::Text("Translation");
			
            ImGui::Text("x:"); ImGui::SameLine();
            ImGui::SetNextItemWidth(80.0f);
            if (ImGui::DragFloat("##XTrans", &xValue, 0.5f))
            {
                btPosition.setX(xValue);
				dirty = true;
            }

            ImGui::SameLine();
            ImGui::Text("y:"); ImGui::SameLine();
            ImGui::SetNextItemWidth(80.0f);
            if (ImGui::DragFloat("##YTrans", &yValue, 0.5f))
            {
                btPosition.setY(yValue);
				dirty = true;
            }

            ImGui::SameLine();
            ImGui::Text("z:"); ImGui::SameLine();
            ImGui::SetNextItemWidth(80.0f);
            if (ImGui::DragFloat("##ZTrans", &zValue, 0.5f))
            {
                btPosition.setZ(zValue);
				dirty = true;
            }

            asRigidBody->SetRigidBodyOrigin(btPosition);
			if (dirty) 
			{
				asRigidBody->UpdateRigidBodyTranslation();
			}
            
            
        }
		if (ImGui::Button("Reset RigidBody position"))
		{
			asRigidBody->SetDefaultPosition();
		}
		ImGui::Text("");
		// Resize RigidBody
		bool dirty = false;
		switch (currentShape)
		{
			case 1: //Box shape
			{
				float3 boxSize = asRigidBody->GetBoxSize();

				ImGui::Text("Scale");
				
				ImGui::Text("x:"); ImGui::SameLine();
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::DragFloat("##XSize", &boxSize.x, 0.5f))
				{
					dirty = true;
				}

				ImGui::SameLine();
				ImGui::Text("y:"); ImGui::SameLine();
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::DragFloat("##YSize", &boxSize.y, 0.5f))
				{
					dirty = true;
				}

				ImGui::SameLine();
				ImGui::Text("z:"); ImGui::SameLine();
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::DragFloat("##ZSize", &boxSize.z, 0.5f))
				{
					dirty = true;
				}

				if (dirty)
				{
					asRigidBody->SetBoxSize(boxSize);
					asRigidBody->SetCollisionShape(static_cast<ComponentRigidBody::Shape>(currentShape));
				}
				break;
			}
			case 2: //Sphere shape
			{
				float radius = asRigidBody->GetRadius();
				float factor = asRigidBody->GetFactor();

				ImGui::Text("Scale");
				
				ImGui::Text("Radius:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::DragFloat("##Radius", &radius, 0.5f))
				{
					asRigidBody->SetRadius(radius);
					dirty = true;
				}

				ImGui::Text("Factor:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::DragFloat("##Factor", &factor, 0.5f))
				{
					asRigidBody->SetFactor(factor);
					dirty = true;
				}

				if (dirty)
				{
					asRigidBody->SetCollisionShape(static_cast<ComponentRigidBody::Shape>(currentShape));
				}
				break;
			}
				
			case 3: //Capsule shape
			case 4: //Cone shape
			{
				float radius = asRigidBody->GetRadius();
				float height = asRigidBody->GetHeight();

				ImGui::Text("Scale");
				
				ImGui::Text("Radius:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::DragFloat("##Radius", &radius, 0.5f))
				{
					asRigidBody->SetRadius(radius);
					dirty = true;
				}

				ImGui::Text("Height:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::DragFloat("##Height", &height, 0.5f))
				{
					asRigidBody->SetHeight(height);
					dirty = true;
				}

				if (dirty)
				{
					asRigidBody->SetCollisionShape(static_cast<ComponentRigidBody::Shape>(currentShape));
				}
				break;
			}
				
		}
		if (ImGui::Button("Reset RigidBody size"))
		{
			asRigidBody->SetDefaultSize(static_cast<ComponentRigidBody::Shape>(currentShape));
		}
		

		//RigidBody box info
		if (currentShape > 0)
		{
			btVector3 minPoint, maxPoint;
			rigidBody->getAabb(minPoint, maxPoint);
			ImGui::Text("");
			ImGui::Text("AABB: Min: %f, %f, %f", minPoint.x(), minPoint.y(), minPoint.z());
			ImGui::Text("AABB: Max: %f, %f, %f", maxPoint.x(), maxPoint.y(), maxPoint.z());
		}

		ImGui::Text("");
		ImGui::Text("Proportional Controller");
		ImGui::Separator();

		float KpForce = asRigidBody->GetKpForce();
		float KpTorque = asRigidBody->GetKpTorque();
		bool isUsingPositionController = asRigidBody->GetUsePositionController();
		bool isUsingRotationController = asRigidBody->GetUseRotationController();

		if (ImGui::Checkbox("##Use Position Controller", &isUsingPositionController))
		{
			asRigidBody->SetUsePositionController(isUsingPositionController);
		}

		ImGui::SameLine();
		ImGui::Text("Use Position Controller");

		if (isUsingPositionController)
		{
			if (ImGui::SliderFloat("KP Position", &KpForce, 0.5f, 10.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
			{
				asRigidBody->SetKpForce(KpForce);
			}
		}

		if (ImGui::Checkbox("##Use Rotation Controller", &isUsingRotationController))
		{
			asRigidBody->SetUseRotationController(isUsingRotationController);
		}

		ImGui::SameLine();
		ImGui::Text("Use Rotation Controller");
		if (isUsingRotationController)
		{
			if (ImGui::SliderFloat("KP Rotation", &KpTorque, 0.5f, 10.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
			{
				asRigidBody->SetKpTorque(KpTorque);
			}
		}
		
	}
}
