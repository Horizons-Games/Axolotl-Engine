#include "StdAfx.h"

#include "GameObject/GameObject.h"
#include "GL/glew.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "ModuleUI.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/ModuleFileSystem.h"
#include "DataModels/Program/Program.h"
#include "ComponentVideo.h"
#include "ComponentCanvas.h"
#include "ComponentTransform2D.h"
#include "GameObject/GameObject.h"
#include "AxoLog.h"
#include "FileSystem/Json.h"
#include "Application.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}
#include <iostream>
#include <filesystem>


ComponentVideo::ComponentVideo(bool active, GameObject* owner) :
	Component(ComponentType::VIDEO, active, owner, true),
	loop(false),
	finished(false),
	rotateVertical(false),
	canRotate(false),
	played(false)
{

}

ComponentVideo::~ComponentVideo()
{
}

void ComponentVideo::Init()
{
	// Set GL texture buffer
	glGenTextures(1, &frameTexture);
	glBindTexture(GL_TEXTURE_2D, frameTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#ifdef ENGINE	
	std::filesystem::path fs_path(video->GetAssetsPath());
	std::string extension = fs_path.extension().string();
	if (extension != AVI_VIDEO_EXTENSION)
	{
		canRotate = true;
	}
	OpenVideo(video->GetAssetsPath().c_str());
#else
	/*std::filesystem::path fs_path(video->GetLibraryPath());
	std::string extension = fs_path.extension().string();*/
	std::string extension = video->GetExtension();
	if (extension != AVI_VIDEO_EXTENSION)
	{
		canRotate = true;
	}
	OpenVideo((video->GetLibraryPath() + extension).c_str());
#endif
}

void ComponentVideo::Draw() const
{
	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::SPRITE);
	
	if (!program)
	{
		LOG_ERROR("Sprite shader not found");
		return;
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	program->Activate();

	if (!GetOwner()->HasComponent<ComponentTransform2D>())
	{
		throw ComponentNotFoundException("Owner does not have ComponentTrasnform2D");
	}
	ComponentTransform2D* transform = GetOwner()->GetComponentInternal<ComponentTransform2D>();

	const float4x4& proj = App->GetModule<ModuleCamera>()->GetOrthoProjectionMatrix();
	const float4x4& model = transform->GetGlobalScaledMatrix();
	float4x4 view = float4x4::identity;

	ComponentCanvas* canvas = transform->WhichCanvasContainsMe();
	if (canvas)
	{
		canvas->RecalculateSizeAndScreenFactor();
		float factor = canvas->GetScreenFactor();
		view = view * float4x4::Scale(factor, factor, factor);
	}

	glUniformMatrix4fv(2, 1, GL_TRUE, (const float*) &view);
	glUniformMatrix4fv(1, 1, GL_TRUE, (const float*) &model);
	glUniformMatrix4fv(0, 1, GL_TRUE, (const float*) &proj);

	glBindVertexArray(App->GetModule<ModuleUI>()->GetQuadVAO());

	glActiveTexture(GL_TEXTURE0);
	program->BindUniformFloat4("spriteColor", float4(1.0f, 1.0f, 1.0f, 1.0f));
	program->BindUniformFloat("renderPercentage", 1.0f);
	program->BindUniformInt("direction", 1);
		
	if (initialized)
	{
		program->BindUniformInt("hasDiffuse", 1);
		glBindTexture(GL_TEXTURE_2D, frameTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, frameWidth, frameHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, frameData);
	}
	else program->BindUniformInt("hasDiffuse", 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisable(GL_BLEND);
	program->Deactivate();
	
}

void ComponentVideo::InternalSave(Json& meta)
{
	UID uidVideo = 0;
	std::string assetPath = "";
	if (video)
	{
		meta["videoUID"] = video->GetUID();
		meta["assetPathVideo"] = video->GetAssetsPath().c_str();
	}
	meta["loop"] = loop;
	meta["rotateVertical"] = rotateVertical;
	meta["canBeRotate"] = canRotate;
	
}

void ComponentVideo::InternalLoad(const Json& meta)
{
	loop = meta["loop"];
	std::string path = meta["assetPathVideo"];
	rotateVertical = meta["rotateVertical"];
	canRotate = meta["canBeRotate"];
#ifdef ENGINE	
	bool resourceExists = !path.empty() && App->GetModule<ModuleFileSystem>()->Exists(path.c_str());
	if (resourceExists)
	{
		std::shared_ptr<ResourceVideo> resourceVideo =
			App->GetModule<ModuleResources>()->RequestResource<ResourceVideo>(path);
		if (resourceVideo)
		{
			video = std::move(resourceVideo);
			Init();
		}
	}
#else
	UID uidVideo = meta["videoUID"];
	std::shared_ptr<ResourceVideo> resourceVideo =
		App->GetModule<ModuleResources>()->SearchResource<ResourceVideo>(uidVideo);
	if (resourceVideo)
	{
		video = std::move(resourceVideo);
		Init();
	}
#endif
}

void ComponentVideo::OpenVideo(const char* filePath)
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
	frameData = new uint8_t[frameWidth * frameHeight * 4];
	SetVideoFrameSize(frameWidth, frameHeight);
	memset(frameData, 0, frameWidth * frameHeight * 4);

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
	initialized = true;
}

void ComponentVideo::ReadVideoFrame()
{
	if (initialized)
	{
		int response = -1;
		int error = 0;
		while (error >= 0)
		{
			error = av_read_frame(formatCtx, avPacket);

			if (avPacket->stream_index != videoStreamIndex)
			{
				av_packet_unref(avPacket);
				continue;
			}

			// SEEK to frame 0 -> Restart the video timestamp
			if (error == AVERROR_EOF)
			{
				finished = true;
				if (loop)
				{
					RestartVideo();
					av_packet_unref(avPacket);
				}
				if(rotateVertical)
				{
					avFrame->data[0] += avFrame->linesize[0] * (videoCodecCtx->height - 1);
					avFrame->linesize[0] *= -1;
					avFrame->data[1] += avFrame->linesize[1] * (videoCodecCtx->height / 2 - 1);
					avFrame->linesize[1] *= -1;
					avFrame->data[2] += avFrame->linesize[2] * (videoCodecCtx->height / 2 - 1);
					avFrame->linesize[2] *= -1;
				}

				break;
			}

			response = avcodec_send_packet(videoCodecCtx, avPacket);
			if (response < 0)
			{
				LOG_ERROR("Failed to decode packet");
				// LOG_ERROR("Failed to decode packet: %s.", libav_err2str(response));
				return;
			}

			response = avcodec_receive_frame(videoCodecCtx, avFrame);
			if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
			{
				av_packet_unref(avPacket);
				continue;
			}
			if (response < 0)
			{
				LOG_ERROR("Failed to decode frame");
				// LOG_ERROR("Failed to decode frame: %s.", libav_err2str(response));
				return;
			}

			av_packet_unref(avPacket);
			break;
		}
		if (!scalerCtx)
		{
			// Set SwScaler - Scale frame size + Pixel converter to RGB
			scalerCtx = sws_getContext(frameWidth,
									   frameHeight,
									   videoCodecCtx->pix_fmt,
									   frameWidth,
									   frameHeight,
									   AV_PIX_FMT_RGBA,
									   SWS_FAST_BILINEAR,
									   nullptr,
									   nullptr,
									   nullptr);

			if (!scalerCtx)
			{
				LOG_ERROR("Couldn't initialise SwScaler.");
				return;
			}
		}


		if (rotateVertical)
		{
			avFrame->data[0] += avFrame->linesize[0] * (videoCodecCtx->height - 1);
			avFrame->linesize[0] *= -1;
			avFrame->data[1] += avFrame->linesize[1] * (videoCodecCtx->height / 2 - 1);
			avFrame->linesize[1] *= -1;
			avFrame->data[2] += avFrame->linesize[2] * (videoCodecCtx->height / 2 - 1);
			avFrame->linesize[2] *= -1;
		}


		uint8_t* dest[4] = { frameData, nullptr, nullptr, nullptr };
		int linSize[4] = { frameWidth * 4, 0, 0, 0 };
		sws_scale(scalerCtx, avFrame->data, avFrame->linesize, 0, frameHeight, dest, linSize);
	}
}

void ComponentVideo::SetVideoFrameSize(int width, int height)
{
	
	ComponentTransform2D* transform = GetOwner()->GetComponentInternal<ComponentTransform2D>();
	transform->SetSize(float2((float) width, (float) height));
}

void ComponentVideo::RestartVideo()
{
	avio_seek(formatCtx->pb, 0, SEEK_SET);
	if (av_seek_frame(formatCtx, videoStreamIndex, -1, 0) >= 0)
	{
	}
}