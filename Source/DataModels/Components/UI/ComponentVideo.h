#pragma once
#include "Auxiliar/Generics/Drawable.h"
#include "DataModels/Components/Component.h"
#include "Resources/ResourceVideo.h"

struct SwsContext;

class ComponentVideo : public Component, public Drawable
{
public:
	ComponentVideo(bool active, GameObject* owner);
	~ComponentVideo() override;
	ComponentVideo(const ComponentVideo& toCopy);
	void SetLoop(bool loop);
	bool IsLooping();
	bool CanBeRotate();
	void SetRotateVertical(bool rotateVertical);
	bool CanRotateVertical();
	void SetVideo(const std::shared_ptr<ResourceVideo>& video);
	void SetPlayAtStart(bool playAtStart);
	bool GetPlayAtStart();
	std::shared_ptr<ResourceVideo> GetVideo() const;
	void SetNativeVideoFrameSize();
	void UpdateVideoFrame();
	void Draw() const override;
	void Play();
	void Pause();
	void Stop();
	void RestartVideo();
	bool isPlayed();
	bool isPlayAtStart();

private:
	void ReadVideoFrame();


	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;
	std::shared_ptr<ResourceVideo> video;

	SwsContext* scalerCtx = nullptr;
	uint8_t* frameData;
	void SetVideoFrameSize(int width, int height);

	bool loop;
	bool rotateVertical;
	bool finished;
	bool played;
	bool firstFrame;
	bool playAtStart;
};

inline void ComponentVideo::SetRotateVertical(bool rotateVertical)
{
	this->rotateVertical = rotateVertical;
}

inline bool ComponentVideo::CanRotateVertical()
{
	return rotateVertical;
}

inline bool ComponentVideo::IsLooping()
{
	return this->loop;
}


inline void ComponentVideo::SetLoop(bool loop)
{
	this->loop = loop;
}

inline void ComponentVideo::SetPlayAtStart(bool playAtStart)
{
	this->playAtStart = playAtStart;
}

inline bool ComponentVideo::GetPlayAtStart()
{
	return playAtStart;
}

inline std::shared_ptr<ResourceVideo> ComponentVideo::GetVideo() const
{
	return this->video;
}

inline void ComponentVideo::Play()
{
	this->played = true;
}

inline void ComponentVideo::Pause()
{
	this->played = false;
}