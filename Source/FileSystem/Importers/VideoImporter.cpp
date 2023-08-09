#include "StdAfx.h"

#include "VideoImporter.h"

#include "Application.h"
#include "FileSystem/Json.h"
#include "FileSystem/ModuleFileSystem.h"
#include "Resources/ResourceVideo.h"
#include "AxoLog.h"

#include "Defines/ExtensionDefines.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
}


VideoImporter::VideoImporter()
{
}

VideoImporter::~VideoImporter()
{
}

//void VideoImporter::Import(const char* filePath, std::shared_ptr<ResourceVideo> resource)
//{
//	// Initialize FFmpeg libraries
//	//av_register_all();
//	avformat_network_init();
//	std::string outputFilePath = resource->GetLibraryPath() + MP3_VIDEO_EXTENSION;
//	resource->GetFileName();
//	AVFormatContext* formatContext = nullptr;
//	// Open the input video file
//	if (avformat_open_input(&formatContext, filePath, nullptr, nullptr) != 0)
//	{
//		LOG_ERROR("Error opening input file.");
//	}
//	else
//	{
//		// Retrieve stream information
//		if (avformat_find_stream_info(formatContext, nullptr) < 0)
//		{
//			LOG_ERROR("Error finding stream information.");
//			avformat_close_input(&formatContext);
//		}
//		AVCodec* codec = nullptr;
//		int audioStreamIndex = -1;
//
//		// Find the audio stream in the format context
//		for (unsigned int i = 0; i < formatContext->nb_streams; i++)
//		{
//			if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
//			{
//				audioStreamIndex = i;
//				codec = avcodec_find_decoder(formatContext->streams[i]->codecpar->codec_id);
//				if (!codec)
//				{
//					LOG_ERROR("Unsupported codec.");
//					avformat_close_input(&formatContext);
//				}
//				break;
//			}
//		}
//		if (audioStreamIndex == -1)
//		{
//			LOG_ERROR("No audio stream found in the input file.");
//			avformat_close_input(&formatContext);
//		}
//
//		AVCodecContext* codecContext = avcodec_alloc_context3(codec);
//		if (!codecContext)
//		{
//			LOG_ERROR("Failed to allocate codec context.");
//			avformat_close_input(&formatContext);
//		}
//		
//
//		// Copy codec parameters to the codec context
//		if (avcodec_parameters_to_context(codecContext, formatContext->streams[audioStreamIndex]->codecpar) < 0)
//		{
//			LOG_ERROR("Failed to copy codec parameters to codec context.");
//			avcodec_free_context(&codecContext);
//			avformat_close_input(&formatContext);
//		}
//
//		// Open the codec
//		if (avcodec_open2(codecContext, codec, nullptr) < 0)
//		{
//			LOG_ERROR("Failed to open codec.");
//			avcodec_free_context(&codecContext);
//			avformat_close_input(&formatContext);
//		}
//		// Open the output audio file for writing
//		FILE* outputFile = fopen(outputFilePath.c_str(), "wb");
//		if (!outputFile)
//		{
//			LOG_ERROR("Failed to open output audio file.");   
//			avcodec_free_context(&codecContext);
//			avformat_close_input(&formatContext);
//		}
//
//		AVPacket packet;
//		av_init_packet(&packet);
//		packet.data = nullptr;
//		packet.size = 0;
//
//		// Loop through the audio packets and write audio data to the output file
//		while (av_read_frame(formatContext, &packet) >= 0)
//		{
//			if (packet.stream_index == audioStreamIndex)
//			{
//				// Decode audio packet
//				AVFrame* frame = av_frame_alloc();
//				int response = avcodec_send_packet(codecContext, &packet);
//				if (response < 0)
//				{
//					av_frame_free(&frame);
//					break;
//				}
//
//				response = avcodec_receive_frame(codecContext, frame);
//				if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
//				{
//					av_frame_free(&frame);
//					continue;
//				}
//				else if (response < 0)
//				{
//					av_frame_free(&frame);
//					break;
//				}
//
//				// Write audio data to the output file
//				fwrite(frame->data[0], 1, frame->linesize[0], outputFile);
//
//				av_frame_free(&frame);
//			}
//			av_packet_unref(&packet);
//		}
//
//		// Clean up
//		fclose(outputFile);
//		avcodec_free_context(&codecContext);
//		avformat_close_input(&formatContext);
//	}
//	char* buffer{};
//	unsigned int size;
//	Save(resource, buffer, size);
//	App->GetModule<ModuleFileSystem>()->Save(
//		(resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, 0);
//	delete buffer;
//}

void VideoImporter::Import(const char* filePath, std::shared_ptr<ResourceVideo> resource)
{

	/*const char* input_filename = filePath;
	const char* output_filename = (resource->GetLibraryPath() + std::string(WAV_AUDIO_EXTENSION)).c_str();*/
	LOG_DEBUG(filePath);
	const char* input_filename = "Assets/Videos/opening_preview.mp4";
	const char* output_filename = "corrected_audio.wav";

	AVFormatContext* input_format_ctx = nullptr;
	if (avformat_open_input(&input_format_ctx, input_filename, nullptr, nullptr) < 0)
	{
		LOG_ERROR("Could not open input file.");
		return;
	}

	if (avformat_find_stream_info(input_format_ctx, nullptr) < 0)
	{
		LOG_ERROR("Could not find stream information.");
		return;
	}

	int audio_stream_index = av_find_best_stream(input_format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	if (audio_stream_index < 0)
	{
		LOG_ERROR("Could not find audio stream.");
		return;
	}

	AVStream* input_stream = input_format_ctx->streams[audio_stream_index];
	AVCodecParameters* audio_codecpar = input_stream->codecpar;
	const AVCodec* audio_codec = avcodec_find_decoder(audio_codecpar->codec_id);
	AVCodecContext* audio_codec_ctx = avcodec_alloc_context3(audio_codec);
	avcodec_parameters_to_context(audio_codec_ctx, audio_codecpar);
	avcodec_open2(audio_codec_ctx, audio_codec, nullptr);

	AVFormatContext* output_format_ctx = nullptr;
	avformat_alloc_output_context2(&output_format_ctx, nullptr, nullptr, output_filename);
	AVStream* output_stream = avformat_new_stream(output_format_ctx, nullptr);

	AVCodecParameters* output_codecpar = output_stream->codecpar;
	output_codecpar->codec_id = AV_CODEC_ID_PCM_S16LE;
	output_codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
	output_codecpar->channels = 2;
	output_codecpar->channel_layout = AV_CH_LAYOUT_STEREO;
	output_codecpar->sample_rate = 44100;

	if (!(output_format_ctx->oformat->flags & AVFMT_NOFILE))
	{
		if (avio_open(&output_format_ctx->pb, output_filename, AVIO_FLAG_WRITE) < 0)
		{
			LOG_ERROR("Could not open output file.");
			return;
		}
	}

	int ret = avformat_write_header(output_format_ctx, nullptr);
	if (ret < 0)
	{
		LOG_ERROR("Could not write output header.");
		return;
	}

	AVPacket packet;
	AVFrame* frame = av_frame_alloc();

	SwrContext* swr_ctx = swr_alloc_set_opts(nullptr,
											 AV_CH_LAYOUT_STEREO,
											 AV_SAMPLE_FMT_S16,
											 44100,
											 audio_codec_ctx->channel_layout,
											 (enum AVSampleFormat) audio_codec_ctx->sample_fmt,
											 audio_codec_ctx->sample_rate,
											 0,
											 nullptr);
	int err = swr_init(swr_ctx);
	//swr_init(swr_ctx);

	while (av_read_frame(input_format_ctx, &packet) >= 0)
	{
		if (packet.stream_index == audio_stream_index)
		{
			if (avcodec_send_packet(audio_codec_ctx, &packet) < 0)
			{
				LOG_ERROR("Error sending packet to codec.");
				return;
			}

			while (avcodec_receive_frame(audio_codec_ctx, frame) == 0)
			{
				AVPacket* output_packet = av_packet_alloc();
				// av_init_packet(output_packet);

				output_packet->data = nullptr; // packet data will be allocated by the encoder
				output_packet->size = 0;

				uint8_t* resampled_data = nullptr;
				int resampled_data_size =
					av_samples_alloc(&resampled_data, nullptr, 2, frame->nb_samples, AV_SAMPLE_FMT_S16, 0);
				swr_convert(swr_ctx,
							&resampled_data,
							resampled_data_size,
							(const uint8_t**) frame->extended_data,
							frame->nb_samples);

				// av_init_packet(&output_packet);
				output_packet->data = resampled_data; // Copy the resampled data
				output_packet->size = resampled_data_size;

				// Set the packet's PTS, DTS, and duration
				output_packet->pts = frame->pts;
				output_packet->dts = frame->pkt_dts;
				AVRational time_base = { 1, audio_codec_ctx->sample_rate };
				output_packet->duration = av_rescale_q(frame->nb_samples, time_base, output_stream->time_base);

				output_packet->stream_index = 0;
				av_packet_rescale_ts(output_packet, input_stream->time_base, output_stream->time_base);
				av_interleaved_write_frame(output_format_ctx, output_packet);
				av_freep(&resampled_data);
			}
		}
		av_packet_unref(&packet);
	}

	av_write_trailer(output_format_ctx);

	if (output_format_ctx && !(output_format_ctx->oformat->flags & AVFMT_NOFILE))
	{
		avio_closep(&output_format_ctx->pb);
	}

	swr_free(&swr_ctx);
	av_frame_free(&frame);
	avcodec_free_context(&audio_codec_ctx);
	avformat_free_context(output_format_ctx);
	avformat_close_input(&input_format_ctx);
	//char* buffer{};
	//unsigned int size;
	//Save(resource, buffer, size);
	//App->GetModule<ModuleFileSystem>()->Save(
	//	(resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, 0);
	//delete buffer;
}

void VideoImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceVideo> resource)
{
}

void VideoImporter::Save(const std::shared_ptr<ResourceVideo>& resource, char*& fileBuffer, unsigned int& size)
{
	size = sizeof(unsigned char) * (resource->GetLibraryPath() + WAV_AUDIO_EXTENSION).size();
	char* cursor = new char[size];
	memcpy(cursor, (resource->GetLibraryPath() + WAV_AUDIO_EXTENSION).c_str(), size);
}