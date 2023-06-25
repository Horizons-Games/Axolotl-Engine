#pragma once

struct State;
class ResourceAnimation;

class AnimationController
{
public:
	AnimationController();
	~AnimationController();

	void Play(State* resource, bool loop);
	void Stop();

	void Update();

	bool GetTransform(const std::string& name, float3& pos, Quat& rot);
	bool GetPlay();
	bool GetLoop();

private:
	float3 Interpolate(const float3& first, const float3& second, float lambda);
	Quat Interpolate(const Quat& first, const Quat& second, float lambda);

	float currentTime;
	bool isLooping;
	bool isPlaying;
	const State* stateResource;
	std::shared_ptr<ResourceAnimation> resource;
};

inline bool AnimationController::GetPlay()
{
	return isPlaying;
}

inline bool AnimationController::GetLoop()
{
	return isLooping;
}
