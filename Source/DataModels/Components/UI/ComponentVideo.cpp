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
	played(false),
	firstFrame(true)
{

}

ComponentVideo::~ComponentVideo()
{
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
		
	if (video)
	{
		video->Load();
		program->BindUniformInt("hasDiffuse", 1);
		glBindTexture(GL_TEXTURE_2D, video->GetGlFrameBuffer());
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA8, video->GetFrameWidth(), video->GetFrameHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, frameData);
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
	meta["playAtStart"] = playAtStart;
	
}

void ComponentVideo::InternalLoad(const Json& meta)
{
	loop = meta["loop"];
	std::string path = meta["assetPathVideo"];
	rotateVertical = meta["rotateVertical"];
	playAtStart = meta["playAtStart"];
#ifdef ENGINE	
	bool resourceExists = !path.empty() && App->GetModule<ModuleFileSystem>()->Exists(path.c_str());
	if (resourceExists)
	{
		std::shared_ptr<ResourceVideo> resourceVideo =
			App->GetModule<ModuleResources>()->RequestResource<ResourceVideo>(path);
		if (resourceVideo)
		{
			SetVideo(resourceVideo);
		}
	}
#else
	UID uidVideo = meta["videoUID"];
	std::shared_ptr<ResourceVideo> resourceVideo =
		App->GetModule<ModuleResources>()->SearchResource<ResourceVideo>(uidVideo);
	if (resourceVideo)
	{
		SetVideo(resourceVideo);
	}
#endif
}

void ComponentVideo::UpdateVideoFrame()
{
	if (played)
	{
		ReadVideoFrame();
	}
}

void ComponentVideo::ReadVideoFrame()
{
	if (video)
	{
		video->Load();
		int response = -1;
		int error = 0;
		while (error >= 0)
		{
			error = av_read_frame(video->GetFormat(), video->GetAvPacket());

			if (video->GetAvPacket()->stream_index != video->GetVideoStream())
			{
				av_packet_unref(video->GetAvPacket());
				continue;
			}

			// SEEK to frame 0 -> Restart the video timestamp
			if (error == AVERROR_EOF)
			{
				finished = true;
				if (loop)
				{
					RestartVideo();
					av_packet_unref(video->GetAvPacket());
				}
				if(video->GetCanRotate() && rotateVertical)
				{
					video->GetAvFrame()->data[0] += video->GetAvFrame()->linesize[0] * (video->GetVideoCodec()->height - 1);
					video->GetAvFrame()->linesize[0] *= -1;
					video->GetAvFrame()->data[1] += video->GetAvFrame()->linesize[1] * (video->GetVideoCodec()->height / 2 - 1);
					video->GetAvFrame()->linesize[1] *= -1;
					video->GetAvFrame()->data[2] += video->GetAvFrame()->linesize[2] * (video->GetVideoCodec()->height / 2 - 1);
					video->GetAvFrame()->linesize[2] *= -1;
				}

				break;
			}

			response = avcodec_send_packet(video->GetVideoCodec(), video->GetAvPacket());
			if (response < 0)
			{
				LOG_ERROR("Failed to decode packet");
				// LOG_ERROR("Failed to decode packet: %s.", libav_err2str(response));
				return;
			}

			response = avcodec_receive_frame(video->GetVideoCodec(), video->GetAvFrame());
			if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
			{
				av_packet_unref(video->GetAvPacket());
				continue;
			}
			if (response < 0)
			{
				LOG_ERROR("Failed to decode frame");
				// LOG_ERROR("Failed to decode frame: %s.", libav_err2str(response));
				return;
			}

			av_packet_unref(video->GetAvPacket());
			break;
		}
		if (!scalerCtx)
		{
			// Set SwScaler - Scale frame size + Pixel converter to RGB
			scalerCtx = sws_getContext(video->GetFrameWidth(),
									   video->GetFrameHeight(),
									   video->GetVideoCodec()->pix_fmt,
									   video->GetFrameWidth(),
									   video->GetFrameHeight(),
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


		if (video->GetCanRotate() && rotateVertical)
		{
			video->GetAvFrame()->data[0] += video->GetAvFrame()->linesize[0] * (video->GetVideoCodec()->height - 1);
			video->GetAvFrame()->linesize[0] *= -1;
			video->GetAvFrame()->data[1] += video->GetAvFrame()->linesize[1] * (video->GetVideoCodec()->height / 2 - 1);
			video->GetAvFrame()->linesize[1] *= -1;
			video->GetAvFrame()->data[2] += video->GetAvFrame()->linesize[2] * (video->GetVideoCodec()->height / 2 - 1);
			video->GetAvFrame()->linesize[2] *= -1;
		}


		uint8_t* dest[4] = { frameData, nullptr, nullptr, nullptr };
		int linSize[4] = { video->GetFrameWidth() * 4, 0, 0, 0 };
		sws_scale(scalerCtx,
				  video->GetAvFrame()->data,
				  video->GetAvFrame()->linesize,
				  0,
				  video->GetFrameHeight(),
				  dest,
				  linSize);
	}
}

void ComponentVideo::SetVideoFrameSize(int width, int height)
{
	
	ComponentTransform2D* transform = GetOwner()->GetComponentInternal<ComponentTransform2D>();
	transform->SetSize(float2((float) width, (float) height));
}

void ComponentVideo::Stop()
{
	RestartVideo();
	ReadVideoFrame();
	played = false;
}

void ComponentVideo::RestartVideo()
{
	avio_seek(video->GetFormat()->pb, 0, SEEK_SET);
	if (av_seek_frame(video->GetFormat(), video->GetVideoStream(), -1, 0) >= 0)
	{
	}
}

bool ComponentVideo::isPlayed()
{
	return played;
}

bool ComponentVideo::isPlayAtStart()
{
	return playAtStart;
}

bool ComponentVideo::GetCanBeRotate()
{
	return video ? video->GetCanRotate() : false;
}

void ComponentVideo::SetVideo(const std::shared_ptr<ResourceVideo>& newVideo)
{
	this->video = std::move(newVideo);
	if (video)
	{
		video->Load();
		frameData = new uint8_t[video->GetFrameWidth() * video->GetFrameHeight() * 4];
		SetVideoFrameSize(video->GetFrameWidth(), video->GetFrameHeight());
		memset(frameData, 0, video->GetFrameWidth() * video->GetFrameHeight() * 4);
		ReadVideoFrame();
	}
	played = false;
	finished = false;
}