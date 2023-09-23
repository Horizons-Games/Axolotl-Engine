#include "StdAfx.h"
#include "ResourceVideo.h"
#include "GL/glew.h"
#include "Defines/ExtensionDefines.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}

#include <filesystem>
ResourceVideo::ResourceVideo(UID resourceUID,
										   const std::string& fileName,
										   const std::string& assetsPath,
										   const std::string& libraryPath):
	Resource(resourceUID, fileName, assetsPath, libraryPath),
	frameRate(0),
	numFrames(0)
{
}

ResourceVideo::~ResourceVideo()
{
	Unload();
}

void ResourceVideo::SaveImporterOptions(Json& meta)
{
}

void ResourceVideo::LoadImporterOptions(Json& meta)
{
}

void ResourceVideo::SaveLoadOptions(Json& meta)
{
}

void ResourceVideo::LoadLoadOptions(Json& meta)
{
}

void ResourceVideo::InternalLoad()
{
	// Set GL texture buffer
	glGenTextures(1, &glFrameBuffer);
	glBindTexture(GL_TEXTURE_2D, glFrameBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (extension != AVI_VIDEO_EXTENSION)
	{
		canRotate = true;
	}
#ifdef ENGINE
	
	OpenVideo(GetAssetsPath().c_str());
#else
	OpenVideo((GetLibraryPath() + extension).c_str());
#endif
}

void ResourceVideo::InternalUnload()
{
	glDeleteTextures(1, &glFrameBuffer);
	glFrameBuffer = 0;
}

void ResourceVideo::OpenVideo(const char* filePath)
{
	// Open video file
	formatCtx = avformat_alloc_context();
	if (!formatCtx)
	{
		LOG_ERROR("Couldn't allocate AVFormatContext.");
		return;
	}
	if (avformat_open_input(&formatCtx, filePath, nullptr, nullptr) != 0)
	{
		LOG_ERROR("Couldn't open video file.");
		return;
	}

	// DECODING VIDEO
	// Find a valid video stream in the file
	AVCodecParameters* videoCodecParams;
	AVCodec* videoDecoder;
	videoStreamIndex = -1;

	videoStreamIndex = av_find_best_stream(formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	if (videoStreamIndex < 0)
	{
		LOG_ERROR("Couldn't find valid video stream inside file.");
		return;
	}

	// Find an appropiate video decoder
	videoCodecParams = formatCtx->streams[videoStreamIndex]->codecpar;
	videoDecoder = avcodec_find_decoder(videoCodecParams->codec_id);
	if (!videoDecoder)
	{
		LOG_ERROR("Couldn't find valid video decoder.");
		return;
	}

	// Set up a video codec context for the decoder
	videoCodecCtx = avcodec_alloc_context3(videoDecoder);
	if (!videoCodecCtx)
	{
		LOG_ERROR("Couldn't allocate AVCodecContext.");
		return;
	}
	if (avcodec_parameters_to_context(videoCodecCtx, videoCodecParams) < 0)
	{
		LOG_ERROR("Couldn't initialise AVCodecContext.");
		return;
	}
	if (avcodec_open2(videoCodecCtx, videoDecoder, nullptr) < 0)
	{
		LOG_ERROR("Couldn't open video codec.");
		return;
	}

	// Set video parameters and Allocate frame buffer
	frameWidth = videoCodecParams->width;
	frameHeight = videoCodecParams->height;

	// Allocate memory for packets and frames
	avPacket = av_packet_alloc();
	if (!avPacket)
	{
		LOG_ERROR("Couldn't allocate AVPacket.");
		return;
	}
	avFrame = av_frame_alloc();
	if (!avFrame)
	{
		LOG_ERROR("Couldn't allocate AVFrame.");
		return;
	}
}