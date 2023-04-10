#include "AnimationController.h"
#include "Application.h"

#include "Resources/ResourceAnimation.h"

AnimationController::AnimationController(ResourceAnimation* resource)
{
	currentTime = 0.0f;
	loop = true;
    play = false;
	this->resource = resource;
}

AnimationController::~AnimationController()
{
}

void AnimationController::Play()
{
    play = true;
}

void AnimationController::Stop()
{
    play = false;
}

void AnimationController::Update()
{
    if (play)
    {
        float duration = resource->GetDuration();

        currentTime += App->GetDeltaTime();
        if (currentTime > duration)
        {
            if (loop)
            {
                currentTime = 0.0f;
            }
            else
            {
                currentTime = duration;
            }
        }
    }
}

void AnimationController::GetTransform(char* name, float3& pos, Quat& rot)
{
    ResourceAnimation::Channel* channel = resource->GetChannel(name);
    float currentSample = (currentTime * (channel->positions.size() - 1)) / resource->GetDuration();

    int first = floor(currentSample);
    int second = ceil(currentSample);

    float3 firstPos = channel->positions[first];
    float3 secondPos = channel->positions[second];;
    Quat firstQuat = channel->rotations[first];
    Quat secondQuat = channel->rotations[second];
    float lambda = currentSample - first;

    pos = Interpolate(firstPos, secondPos, lambda);
    rot = Interpolate(firstQuat, secondQuat, lambda);
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
