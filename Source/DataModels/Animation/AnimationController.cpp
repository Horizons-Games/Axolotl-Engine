#include "AnimationController.h"
#include "Application.h"

#include "Resources/ResourceAnimation.h"

AnimationController::AnimationController() :
    currentTime(0.0f), isLooping(true), isPlaying(false), resource(nullptr)
{
}

AnimationController::~AnimationController()
{
}

void AnimationController::Play(const std::shared_ptr<ResourceAnimation>& resource, bool loop)
{
    this->resource = resource;
    isLooping = loop;
    isPlaying = true;
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
        float duration = resource->GetDuration();

        currentTime += App->GetDeltaTime() * 10;
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

bool AnimationController::GetTransform(const std::string& name, float3& pos, Quat& rot)
{
    ResourceAnimation::Channel* channel = resource->GetChannel(name);

    if (channel)
    {
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
