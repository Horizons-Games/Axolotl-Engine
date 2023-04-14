#pragma once

#include "Math/Quat.h"
#include "Math/float3.h"

#include <memory>

class ResourceAnimation;

class AnimationController
{
public:
	AnimationController();
	~AnimationController();

	void Play(std::shared_ptr<ResourceAnimation> resource, bool loop);
	void Stop();
	
	void Update();

	bool GetTransform(char* name, float3& pos, Quat& rot);

private:
	float3 Interpolate(const float3& first, const float3& second, float lambda);
	Quat Interpolate(const Quat& first, const Quat& second, float lambda);

	float currentTime;
	bool loop;
	bool play;
	std::shared_ptr<ResourceAnimation> resource;
};

