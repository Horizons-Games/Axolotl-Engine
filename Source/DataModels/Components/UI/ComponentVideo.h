#pragma once
#include "Auxiliar/Generics/Drawable.h"
#include "DataModels/Components/Component.h"
#include "Resources/ResourceVideo.h"

struct AVFormatContext;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;
struct SwsContext;

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
	void SetVideo(const std::shared_ptr<ResourceVideo>& video);
	std::shared_ptr<ResourceVideo> GetVideo() const;
	void ReadVideoFrame();
	void Draw() const override;
	void Play();
	void Pause();


private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;
	void OpenVideo(const char* filePath);
	void RestartVideo();
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