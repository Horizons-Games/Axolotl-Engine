#pragma once

#include "Resource.h"

#include "Math/float3.h"
#include "Math/Quat.h"

class ResourceAnimation : public Resource
{
public:
	struct Channel
	{
		char* name;
		unsigned int numPositions;
		unsigned int numRotations;
		std::vector<float3> positions;
		std::vector<Quat> rotations;
	};

public:
	ResourceAnimation(UID resourceId,
		const std::string& fileName,
		const std::string& assetsPath,
		const std::string& libraryPath);
	~ResourceAnimation();

	ResourceType GetType();

protected:
	void InternalLoad() override;
	void InternalUnload() override;

private:
	std::vector<Channel*> channels;
	float duration;
};

inline ResourceType ResourceAnimation::GetType()
{
	return ResourceType::Animation;
}