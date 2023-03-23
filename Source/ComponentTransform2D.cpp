#include "ComponentTransform2D.h"
#include "FileSystem/Json.h"

ComponentTransform2D::ComponentTransform2D(const bool active, GameObject* owner)
	: Component(ComponentType::TRANSFORM2D, active, owner, true)
{
}

ComponentTransform2D::~ComponentTransform2D()
{
}

void ComponentTransform2D::Update()
{

}

void ComponentTransform2D::SaveOptions(Json& meta)
{
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = static_cast<bool>(active);
	meta["removed"] = static_cast<bool>(canBeRemoved);

	meta["localPositionX"] = static_cast<float>(localPosition.x);
	meta["localPositionY"] = static_cast<float>(localPosition.y);
	meta["localPositionZ"] = static_cast<float>(localPosition.z);

	meta["eulerAnglesX"] = static_cast<float>(eulerAngles.x);
	meta["eulerAnglesY"] = static_cast<float>(eulerAngles.y);
	meta["eulerAnglesZ"] = static_cast<float>(eulerAngles.z);

	meta["localScaleX"] = static_cast<float>(localScale.x);
	meta["localScaleY"] = static_cast<float>(localScale.y);
	meta["localScaleZ"] = static_cast<float>(localScale.z);
}

void ComponentTransform2D::LoadOptions(Json& meta)
{
	type = GetTypeByName(meta["type"]);
	active = static_cast<bool>(meta["active"]);
	canBeRemoved = static_cast<bool>(meta["removed"]);

	localPosition.x = static_cast<float>(meta["localPositionX"]);
	localPosition.y = static_cast<float>(meta["localPositionY"]);
	localPosition.z = static_cast<float>(meta["localPositionZ"]);

	eulerAngles.x = static_cast<float>(meta["localRotationX"]);
	eulerAngles.y = static_cast<float>(meta["localRotationY"]);
	eulerAngles.z = static_cast<float>(meta["localRotationZ"]);

	localScale.x = static_cast<float>(meta["localScaleX"]);
	localScale.y = static_cast<float>(meta["localScaleY"]);
	localScale.z = static_cast<float>(meta["localScaleZ"]);
}