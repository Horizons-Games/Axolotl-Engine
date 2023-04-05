#pragma once

#include "Resource.h"

#include "Math/float3.h"
#include "Math/Quat.h"

#include <unordered_map>


class ResourceAnimation : public Resource
{
public:
struct Channel
{
	std::vector<float3> positions;
	std::vector<Quat> rotations;
};

public:
	ResourceAnimation(UID resourceId,
		const std::string& fileName,
		const std::string& assetsPath,
		const std::string& libraryPath);
	~ResourceAnimation() override;

	ResourceType GetType() const override;
	std::unordered_map<std::string, Channel*> GetChannels() const;
	unsigned int GetNumChannels() const;
	double GetDuration() const;
	void SetDuration(double duration);
	void SetChannels(std::unordered_map<std::string, ResourceAnimation::Channel*> channels);

protected:
	void InternalLoad() override {};
	void InternalUnload() override;

private:
	double duration;
	std::unordered_map<std::string, Channel*> channels;
};

inline ResourceType ResourceAnimation::GetType() const
{
	return ResourceType::Animation;
}

inline double ResourceAnimation::GetDuration() const
{
	return duration;
}

inline std::unordered_map<std::string, ResourceAnimation::Channel*> ResourceAnimation::GetChannels() const
{
	return channels;
}

inline unsigned int ResourceAnimation::GetNumChannels() const
{
	return channels.size();
}

inline void ResourceAnimation::SetDuration(double duration)
{
	this->duration = duration;
}

inline void ResourceAnimation::SetChannels(std::unordered_map<std::string, ResourceAnimation::Channel*> channels)
{
	this->channels = channels;
}