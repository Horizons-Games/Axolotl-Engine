#include "StdAfx.h"
#include "UIVideoControl.h"

#include "Application.h"
#include "ModuleAudio.h"
#include "Auxiliar/Audio/AudioData.h"

#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentVideo.h"
#include "Components/ComponentScript.h"
#include "SceneLoadingScript.h"

REGISTERCLASS(UIVideoControl);

UIVideoControl::UIVideoControl() : Script(), imageTransform(nullptr), videoObject(nullptr), playingVideo(false),
playVideo(false), changeVideoTransform(false), object(nullptr), isFirstCutscene(true)
{
	REGISTER_FIELD(playVideo, bool);
	REGISTER_FIELD(videoObject, GameObject*);
	REGISTER_FIELD(object, GameObject*);
	REGISTER_FIELD(changeVideoTransform, bool);
	REGISTER_FIELD(startPosition, float3);
	REGISTER_FIELD(endPosition, float3);
	REGISTER_FIELD(loadingScreenScript, SceneLoadingScript*);
	REGISTER_FIELD(isFirstCutscene, bool);
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
			App->GetModule<ModuleAudio>()->StopAllSFX();
			if (isFirstCutscene)
			{
				AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::CUTSCENE01);
			}
			else
			{
				AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::CUTSCENE02);
			}

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

	if (object)
	{
		if (!object->IsEnabled() && currentPositon.y != endPosition.y)
		{
			imageTransform->SetRotation(endPosition);
			imageTransform->CalculateMatrices();
		}
		else if (object->IsEnabled() && currentPositon.y != startPosition.y)
		{
			imageTransform->SetRotation(startPosition);
			imageTransform->CalculateMatrices();
		}
	}
	else
	{
		if (currentPositon.y != endPosition.y)
		{
			imageTransform->SetRotation(endPosition);
			imageTransform->CalculateMatrices();
		}
		else if (currentPositon.y != startPosition.y)
		{
			imageTransform->SetRotation(startPosition);
			imageTransform->CalculateMatrices();
		}
	}
}
