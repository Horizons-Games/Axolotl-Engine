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

		bool isKinematic = asRigidBody->GetIsKinematic();

		ImGui::Text("Is Kinematic");
		ImGui::SameLine();
		if (ImGui::Checkbox("##Is Kinematic", &isKinematic))
		{
			asRigidBody->SetIsKinematic(isKinematic);
			asRigidBody->SetUpMobility();
		}

		bool isStatic = asRigidBody->IsStatic();

		ImGui::Text("Is Static");
		ImGui::SameLine();
		if (ImGui::Checkbox("##Is Static", &isStatic))
		{
			if (isStatic)
			{
				asRigidBody->SetIsKinematic(false);
			}
			asRigidBody->SetIsStatic(isStatic);
			asRigidBody->SetUpMobility();
		}

		bool isTrigger = asRigidBody->IsTrigger();

		ImGui::Text("Is Static");
		ImGui::SameLine();
		if (ImGui::Checkbox("##Is Trigger", &isTrigger))
		{
			asRigidBody->SetIsTrigger(isTrigger);
		}
		ImGui::Text("");

		bool drawCollider = asRigidBody->GetDrawCollider();

		ImGui::Text("Draw collider");
		ImGui::SameLine();
		if (ImGui::Checkbox("##Draw collider", &drawCollider))
		{
			asRigidBody->SetDrawCollider(drawCollider);
		}

		// Shape
		const char* shapeTypes[] = { "None", "Box", "Sphere", "Capsule", "Cone" /*, "Cylinder", */ };

		int currentShape = static_cast<int>(asRigidBody->GetShape());
		if (ImGui::Combo("Shape", &currentShape, shapeTypes, IM_ARRAYSIZE(shapeTypes)))
		{
			asRigidBody->SetCollisionShape(static_cast<ComponentRigidBody::Shape>(currentShape));
		}

		ImGui::Text("Collider Size");
		ImGui::SameLine();
		if (ImGui::Button("Reset size", ImVec2(120, 0)))
		{
			asRigidBody->SetDefaultSize(static_cast<ComponentRigidBody::Shape>(currentShape));
		}

		// Box shape
		bool dirty = false;
		float radius = asRigidBody->GetRadius();
		float factor = asRigidBody->GetFactor();
		float height = asRigidBody->GetHeight();
		float3 boxSize = asRigidBody->GetBoxSize();
		switch (currentShape)
		{
			case 1:

				ImGui::Text("x:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::DragFloat("##XTrans", &boxSize.x, 0.5f))
				{
					dirty = true;
				}

				ImGui::SameLine();
				ImGui::Text("y:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::DragFloat("##YTrans", &boxSize.y, 0.5f))
				{
					dirty = true;
				}

				ImGui::SameLine();
				ImGui::Text("z:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::DragFloat("##ZTrans", &boxSize.z, 0.5f))
				{
					dirty = true;
				}

				if (dirty)
				{
					asRigidBody->SetBoxSize(boxSize);
					asRigidBody->SetCollisionShape(static_cast<ComponentRigidBody::Shape>(currentShape));
				}
				break;
			case 2:

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
			case 3:
			case 4:

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

		// WIP: Cylinder shape

		if (currentShape > 0)
		{
			btVector3 minPoint, maxPoint;
			rigidBody->getAabb(minPoint, maxPoint);
			ImGui::Text("AABB: Min: %f, %f, %f", minPoint.x(), minPoint.y(), minPoint.z());
			ImGui::Text("AABB: Max: %f, %f, %f", maxPoint.x(), maxPoint.y(), maxPoint.z());
		}

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
			// rigidBody->setDamping(linearDamping, rigidBody->getAngularDamping());
			asRigidBody->SetLinearDamping(linearDamping);
		}

		// Angular damping
		float angularDamping = rigidBody->getAngularDamping();
		if (ImGui::DragFloat("Angular Damping", &angularDamping, 0.01f, 0.0f, 1.0f))
		{
			// rigidBody->setDamping(rigidBody->getLinearDamping(), angularDamping);
			asRigidBody->SetAngularDamping(angularDamping);
		}

		// Restitution
		float restitution = rigidBody->getRestitution();
		if (ImGui::DragFloat("Restitution", &restitution, 0.01f, 0.0f, 1.0f))
		{
			// rigidBody->setRestitution(restitution);
			asRigidBody->SetRestitution(angularDamping);
		}

		float KpForce = asRigidBody->GetKpForce();
		float KpTorque = asRigidBody->GetKpTorque();
		bool isUsingPositionController = asRigidBody->GetUsePositionController();
		bool isUsingRotationController = asRigidBody->GetUseRotationController();

		ImGui::Text("Use Position Controller");
		ImGui::SameLine();
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

		ImGui::Text("Use Rotation Controller");
		ImGui::SameLine();
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
