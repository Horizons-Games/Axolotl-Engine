#pragma once

#include "Math/Quat.h"
#include "Math/float3.h"

class ResourceAnimation;

class AnimationController
{
public:
	AnimationController(ResourceAnimation* resource);
	~AnimationController();

	void Play();
	void Stop();
	void Update();
	void GetTransform(char* name, float3& pos, Quat& rot);

private:
	float3 Interpolate(const float3& first, const float3& second, float lambda);
	Quat Interpolate(const Quat& first, const Quat& second, float lambda);

	unsigned int currentTime;
	bool loop;
	bool play;
	ResourceAnimation* resource;
};

