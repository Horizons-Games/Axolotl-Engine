#pragma once

#include "Resource.h"
#include "ResourceTexture.h"

struct AVFormatContext;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;

class ResourceVideo : virtual public Resource
{
public:
	ResourceVideo(UID resourceUID,
					const std::string& fileName,
					const std::string& assetsPath,
					const std::string& libraryPath);
	virtual ~ResourceVideo() override;
	ResourceType GetType() const override;
	unsigned int GetGlFrameBuffer() const;
	std::string GetExtension() const;
	void SetExtension(std::string extension);
	AVFormatContext* GetFormat() const;
	AVCodecContext* GetVideoCodec() const;
	AVPacket* GetAvPacket() const;
	AVFrame* GetAvFrame() const;
	int GetVideoStream() const;
	int GetFrameWidth() const;
	int GetFrameHeight() const;
	bool GetCanRotate() const;
	void SaveImporterOptions(Json& meta) override;
	void LoadImporterOptions(Json& meta) override;

	void SaveLoadOptions(Json& meta) override;
	void LoadLoadOptions(Json& meta) override;

protected:
	void InternalLoad() override;
	void InternalUnload() override;

	void OpenVideo(const char* filePath);

private:
	unsigned int glFrameBuffer = 0;
	bool canRotate;
	std::string extension;
	unsigned int frameRate;
	unsigned int numFrames;

	AVFormatContext* formatCtx = nullptr;
	AVCodecContext* videoCodecCtx = nullptr;
	AVPacket* avPacket = nullptr;
	AVFrame* avFrame = nullptr;
	int videoStreamIndex;
	int frameWidth;
	int frameHeight;

};

inline ResourceType ResourceVideo::GetType() const
{
	return ResourceType::Video;
}

inline unsigned int ResourceVideo::GetGlFrameBuffer() const
{
	return glFrameBuffer;
}

inline std::string ResourceVideo::GetExtension() const
{
	return this->extension;
}

inline void ResourceVideo::SetExtension(std::string extension)
{
	this->extension = extension;
}

inline AVFormatContext* ResourceVideo::GetFormat() const
{
	return formatCtx;
}

inline AVCodecContext* ResourceVideo::GetVideoCodec() const
{
	return videoCodecCtx;
}

inline AVPacket* ResourceVideo::GetAvPacket() const
{
	return avPacket;
}

inline AVFrame* ResourceVideo::GetAvFrame() const
{
	return avFrame;
}

inline int ResourceVideo::GetVideoStream() const
{
	return videoStreamIndex;
}

inline int ResourceVideo::GetFrameWidth() const
{
	return frameWidth;
}

inline int ResourceVideo::GetFrameHeight() const
{
	return frameHeight;
}

inline bool ResourceVideo::GetCanRotate() const
{
	return canRotate;
}
