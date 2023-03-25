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

	meta["localPositionX"] = static_cast<float>(pos.x);
	meta["localPositionY"] = static_cast<float>(pos.y);
	meta["localPositionZ"] = static_cast<float>(pos.z);

	meta["eulerAnglesX"] = static_cast<float>(eulerAngles.x);
	meta["eulerAnglesY"] = static_cast<float>(eulerAngles.y);
	meta["eulerAnglesZ"] = static_cast<float>(eulerAngles.z);

	meta["localScaleX"] = static_cast<float>(sca.x);
	meta["localScaleY"] = static_cast<float>(sca.y);
	meta["localScaleZ"] = static_cast<float>(sca.z);
}

void ComponentTransform2D::LoadOptions(Json& meta)
{
	type = GetTypeByName(meta["type"]);
	active = static_cast<bool>(meta["active"]);
	canBeRemoved = static_cast<bool>(meta["removed"]);

	pos.x = static_cast<float>(meta["localPositionX"]);
	pos.y = static_cast<float>(meta["localPositionY"]);
	pos.z = static_cast<float>(meta["localPositionZ"]);

	eulerAngles.x = static_cast<float>(meta["localRotationX"]);
	eulerAngles.y = static_cast<float>(meta["localRotationY"]);
	eulerAngles.z = static_cast<float>(meta["localRotationZ"]);

	sca.x = static_cast<float>(meta["localScaleX"]);
	sca.y = static_cast<float>(meta["localScaleY"]);
	sca.z = static_cast<float>(meta["localScaleZ"]);
}