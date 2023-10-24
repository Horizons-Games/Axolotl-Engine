#pragma once

struct State;
class ResourceAnimation;
class ResourceStateMachine;

#define DEFAULT_DURATION -1.0

class AnimationController
{
public:
	AnimationController();
	~AnimationController();

	void Play(State* resource, bool loop, double duration = DEFAULT_DURATION);
	void Stop();

	void Update();

	bool GetTransform(const std::string& name, float3& pos, Quat& rot, bool interpolateTransition = false);
	bool GetPlay();
	bool GetLoop();
	std::string GetStateName();

private:
	float3 Interpolate(const float3& first, const float3& second, float lambda);
	Quat Interpolate(const Quat& first, const Quat& second, float lambda);

	float currentTime;
	float nextTime;
	float fadeDuration;
	float fadeTime;
	bool isLooping;
	bool isPlaying;
	double duration;

	std::shared_ptr<ResourceAnimation> resource;
	std::shared_ptr<ResourceAnimation> nextResource;

	const State* stateResource;
};

inline bool AnimationController::GetPlay()
{
	return isPlaying;
}

inline bool AnimationController::GetLoop()
{
	return isLooping;
}
