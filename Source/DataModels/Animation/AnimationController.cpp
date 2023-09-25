#include "StdAfx.h"

#include "AnimationController.h"
#include "Application.h"

#include "Resources/ResourceStateMachine.h"
#include "Resources/ResourceAnimation.h"

AnimationController::AnimationController() :
	currentTime(0.0f),
	isLooping(true),
	isPlaying(false),
	stateResource(nullptr),
	resource(nullptr)
{
}

AnimationController::~AnimationController()
{
}

void AnimationController::Play(State* stateResource, bool loop, double duration)
{
	if (duration == DEFAULT_DURATION)
	{
		this->stateResource = stateResource;
		resource = std::dynamic_pointer_cast<ResourceAnimation>(stateResource->resource);
		this->duration = resource->GetDuration();
	}
	else
	{
		nextResource = std::dynamic_pointer_cast<ResourceAnimation>(stateResource->resource);
		lastSampleTime = (currentTime / this->duration);
		this->duration = duration;
	}
	isLooping = loop;
	isPlaying = true;
	currentTime = 0;
}

void AnimationController::Stop()
{
	isPlaying = false;
	currentTime = 0;
}

void AnimationController::Update()
{
	if (isPlaying && resource)
	{
		float duration = static_cast<float>(this->duration);

		currentTime += App->GetDeltaTime() * stateResource->speed * 10;
		if (currentTime > duration)
		{
			if (isLooping)
			{
				currentTime = 0.0f;
			}
			else
			{
				isPlaying = false;
			}
		}
	}
}

bool AnimationController::GetTransform(const std::string& name, float3& pos, Quat& rot, bool interpolateTransition)
{
	ResourceAnimation::Channel* channel = resource->GetChannel(name);

	if (channel)
	{
		float time = interpolateTransition? lastSampleTime : (currentTime / this->duration);
		float currentSample = static_cast<float>(time * (channel->positions.size() - 1));

		int first = static_cast<int>(floor(currentSample));
		int second = static_cast<int>(ceil(currentSample));

		float3 firstPos = channel->positions[first];
		float3 secondPos = channel->positions[second];
		
		Quat firstQuat = channel->rotations[first];
		Quat secondQuat = channel->rotations[second];
		float lambda = currentSample - first;

		pos = Interpolate(firstPos, secondPos, lambda);
		rot = Interpolate(firstQuat, secondQuat, lambda);

		if (interpolateTransition)
		{
			ResourceAnimation::Channel* nextChannel = nextResource->GetChannel(name);

			if (!nextChannel)
			{
				return false;
			}

			currentSample = static_cast<float>((currentTime / this->duration));

			first = static_cast<int>(floor(currentSample));

			secondPos = nextChannel->positions[0];

			secondQuat = nextChannel->rotations[0];
			lambda = currentSample - first;

			pos = Interpolate(firstPos, secondPos, lambda);
			rot = Interpolate(firstQuat, secondQuat, lambda);
		}

		return true;
	}

	return false;
}

float3 AnimationController::Interpolate(const float3& first, const float3& second, float lambda)
{
	return first * (1.0f - lambda) + second * lambda;
}

Quat AnimationController::Interpolate(const Quat& first, const Quat& second, float lambda)
{
	Quat result;
	float dot = first.Dot(second);

	if (dot >= 0.0f) // Interpolate through the shortest path
	{
		result.x = first.x * (1.0f - lambda) + second.x * lambda;
		result.y = first.y * (1.0f - lambda) + second.y * lambda;
		result.z = first.z * (1.0f - lambda) + second.z * lambda;
		result.w = first.w * (1.0f - lambda) + second.w * lambda;
	}
	else
	{
		result.x = first.x * (1.0f - lambda) - second.x * lambda;
		result.y = first.y * (1.0f - lambda) - second.y * lambda;
		result.z = first.z * (1.0f - lambda) - second.z * lambda;
		result.w = first.w * (1.0f - lambda) - second.w * lambda;
	}

	result.Normalize();

	return result;
}

std::string AnimationController::GetStateName()
{
	return stateResource->name;
}