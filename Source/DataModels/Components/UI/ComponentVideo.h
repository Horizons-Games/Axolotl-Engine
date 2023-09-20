#pragma once
#include "Auxiliar/Generics/Drawable.h"
#include "DataModels/Components/Component.h"
#include "Resources/ResourceVideo.h"

struct AVFormatContext;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;
struct SwsContext;
class ResourceMesh;

class ComponentVideo : public Component, public Drawable
{
public:
	ComponentVideo(bool active, GameObject* owner);
	~ComponentVideo() override;
	void Init();
	void SetLoop(bool loop);
	bool GetLoop();
	void SetCanBeRotate(bool canRotate);
	bool GetCanBeRotate();
	void SetRotateVertical(bool rotateVertical);
	bool GetRotateVertical();
	void SetUI(bool ui);
	void SetVideo(const std::shared_ptr<ResourceVideo>& video);
	void SetPlayAtStart(bool playAtStart);
	bool GetPlayAtStart();
	bool IsInitialized();
	std::shared_ptr<ResourceVideo> GetVideo() const;
	void ReadVideoFrame();
	void Draw() const override;
	void Play();
	void Pause();
	void RestartVideo();
	bool isPlayed();
	bool isPlayAtStart();

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;
	void OpenVideo(const char* filePath);
	std::shared_ptr<ResourceVideo> video;

	AVFormatContext* formatCtx = nullptr;
	AVCodecContext* videoCodecCtx = nullptr;
	AVPacket* avPacket = nullptr;
	AVFrame* avFrame = nullptr;
	SwsContext* scalerCtx = nullptr;

	unsigned int frameTexture = 0;
	int videoStreamIndex;
	int frameWidth;
	int frameHeight;
	uint8_t* frameData;

	int audioStreamIndex = -1;

	bool initialized = false;

	void SetVideoFrameSize(int width, int height);

	bool loop;
	bool rotateVertical;
	bool finished;
	bool canRotate;
	bool played;
	bool firstFrame;
	bool playAtStart;
	bool ui;
	std::shared_ptr<ResourceMesh> plane;

};

inline void ComponentVideo::SetCanBeRotate(bool canRotate)
{
	this->canRotate = canRotate;
}

inline bool ComponentVideo::GetCanBeRotate()
{
	return this->canRotate;
}

inline void ComponentVideo::SetRotateVertical(bool rotateVertical)
{
	this->rotateVertical = rotateVertical;
}

inline bool ComponentVideo::GetRotateVertical()
{
	return rotateVertical;
}

inline void ComponentVideo::SetUI(bool ui)
{
	this->ui = ui;
}

inline bool ComponentVideo::GetLoop()
{
	return this->loop;
}


inline void ComponentVideo::SetLoop(bool loop)
{
	this->loop = loop;
}

inline void ComponentVideo::SetVideo(const std::shared_ptr<ResourceVideo>& video)
{
	this->video = video;
	Init();
}

inline void ComponentVideo::SetPlayAtStart(bool playAtStart)
{
	this->playAtStart = playAtStart;
}

inline bool ComponentVideo::GetPlayAtStart()
{
	return playAtStart;
}

inline bool ComponentVideo::IsInitialized()
{
	return initialized;
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