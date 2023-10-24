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
		//lastSampleTime = (currentTime / this->duration);
		this->duration = duration;
	}
	isLooping = loop;
	isPlaying = true;
	currentTime = 0.0f;
	nextTime = 0.0f;
	fadeTime = 0.0f;
	fadeDuration = 2.5f;
}

void AnimationController::Stop()
{
	isPlaying = false;
	currentTime = 0.0f;
	nextTime = 0.0f;
	fadeTime = 0.0f;
}

void AnimationController::Update()
{
	if (isPlaying && resource)
	{
		float duration = static_cast<float>(this->duration);
		float delta = App->GetDeltaTime();

		currentTime += delta * stateResource->speed * 10;

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
		
		if (nextResource)
		{
			nextTime += delta * stateResource->speed * 10;

			float fadeLeft = fadeDuration - fadeTime;

			if (nextTime > duration)
			{
				if (isLooping)
				{
					nextTime = 0.0f;
				}
			}

			if (delta <= fadeLeft)
			{
				fadeTime += delta;
			}
			else
			{
				fadeTime = 0.0f;
			}
		}
	}
}

bool AnimationController::GetTransform(const std::string& name, float3& pos, Quat& rot, bool interpolateTransition)
{
	ResourceAnimation::Channel* channel = resource->GetChannel(name);

	if (channel)
	{
		float time = currentTime / this->duration;
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

		if (interpolateTransition & nextResource != nullptr)
		{
			ResourceAnimation::Channel* nextChannel = nextResource->GetChannel(name);

			if (!nextChannel)
			{
				return false;
			}

			float time = nextTime / this->duration;
			currentSample = static_cast<float>(time * (nextChannel->positions.size() - 1));

			first = static_cast<int>(floor(currentSample));
			second = static_cast<int>(ceil(currentSample));

			firstPos = channel->positions[first];
			secondPos = channel->positions[second];

			firstQuat = nextChannel->rotations[first];
			secondQuat = nextChannel->rotations[second];

			lambda = currentSample - first;

			float3 nextPos = Interpolate(firstPos, secondPos, lambda);
			Quat nextRot = Interpolate(firstQuat, secondQuat, lambda);

			float blendWeight = fadeTime / fadeDuration;

			pos = Interpolate(pos, nextPos, blendWeight);
			rot = Interpolate(rot, nextRot, blendWeight);

			//LOG_DEBUG("Difference {}, {}, {}", pos.x - firstPos.x, pos.y - firstPos.y, pos.z - firstPos.z);
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