#pragma once

#include "Resource.h"

#include "Math/float3.h"
#include "Math/Quat.h"

#include <unordered_map>


class ResourceAnimation : virtual public Resource
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

	void SaveImporterOptions(Json& meta) override {};
	void SaveLoadOptions(Json& meta) override {};

	void LoadImporterOptions(Json& meta) override {};
	void LoadLoadOptions(Json& meta) override {};

	std::unordered_map<std::string, Channel*> GetChannels() const;
	Channel* GetChannel(const std::string &name) const;
	unsigned int GetNumChannels() const;
	double GetDuration() const;

	void SetDuration(double duration);
	void SetChannels(const std::unordered_map<std::string, ResourceAnimation::Channel*>& channels);

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

inline ResourceAnimation::Channel* ResourceAnimation::GetChannel(const std::string& name) const
{
	auto it = channels.find(name);
	if (it != channels.end())
	{
		return it->second;
	}
	return nullptr;
}

inline unsigned int ResourceAnimation::GetNumChannels() const
{
	return channels.size();
}

inline void ResourceAnimation::SetDuration(double duration)
{
	this->duration = duration;
}

inline void ResourceAnimation::SetChannels(const std::unordered_map<std::string, ResourceAnimation::Channel*>& channels)
{
	this->channels = channels;
}