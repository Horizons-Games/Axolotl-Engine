#include "StdAfx.h"
#include "UIVideoControl.h"

#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentVideo.h"
#include "Components/ComponentScript.h"
#include "SceneLoadingScript.h"

REGISTERCLASS(UIVideoControl);

UIVideoControl::UIVideoControl() : Script(), imageTransform(nullptr), videoObject(nullptr), playingVideo(false),
playVideo(false), changeVideoTransform(false)
{
	REGISTER_FIELD(playVideo, bool);
	REGISTER_FIELD(videoObject, GameObject*);
	REGISTER_FIELD(changeVideoTransform, bool);
	REGISTER_FIELD(startPosition, float3);
	REGISTER_FIELD(endPosition, float3);
	REGISTER_FIELD(loadingScreenScript, SceneLoadingScript*);
}

void UIVideoControl::Start()
{
	imageTransform = owner->GetComponent<ComponentTransform2D>();

	if (videoObject->HasComponent<ComponentVideo>())
	{
		videoImage = videoObject->GetComponent<ComponentVideo>();
	}
}

void UIVideoControl::Update(float deltaTime)
{
	if (changeVideoTransform)
	{
		ChangeVideoTransform();
	}

	if (playVideo)
	{
		if (!videoImage->isPlayed() && !playingVideo)
		{
			videoImage->Play();
			playingVideo = true;
			LOG_INFO("VIDEO PALYING");
		}
		if (videoImage->IsVideoFinished())
		{
			LOG_INFO("VIDEO FINISHED");

			if (loadingScreenScript)
			{
				LOG_INFO("STARTING LOAD SCRIPT");
#ifndef ENGINE
				loadingScreenScript->StartLoad();
#endif // 
			}
		}
	}
}

void UIVideoControl::ChangeVideoTransform()
{
	currentPositon = imageTransform->GetRotationXYZ();

	if (!videoObject->IsEnabled() && currentPositon.y != endPosition.y)
	{
		imageTransform->SetRotation(endPosition);
		imageTransform->CalculateMatrices();
	}
	else if (videoObject->IsEnabled() && currentPositon.y != startPosition.y)
	{
		imageTransform->SetRotation(startPosition);
		imageTransform->CalculateMatrices();
	}
}
