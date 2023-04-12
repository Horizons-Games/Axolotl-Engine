#include "AnimationImporter.h"

#include "Application.h"

#include "FileSystem/ModuleFileSystem.h"

void AnimationImporter::Import(const char* filePath, std::shared_ptr<ResourceAnimation> resource)
{
	char* loadBuffer;
	App->fileSystem->Load(filePath, loadBuffer);
	Load(loadBuffer, resource);

	char* saveBuffer{};
	unsigned int size;
	Save(resource, saveBuffer, size);
	App->fileSystem->Save((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), saveBuffer, size);

	delete loadBuffer;
	delete saveBuffer;
}

void AnimationImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceAnimation> resource)
{
	unsigned int header[1];
	unsigned int bytes = sizeof(header);
	
	memcpy(header, fileBuffer, bytes);

	fileBuffer += bytes;

	bytes = sizeof(double);

	double* duration = new double;
	memcpy(duration, fileBuffer, bytes);
	resource->SetDuration(*duration);

	delete duration;

	fileBuffer += bytes;

	std::unordered_map<std::string, ResourceAnimation::Channel*> channels;
	channels.reserve(header[0]);

	for (int i = 0; i < header[0]; ++i)
	{
		ResourceAnimation::Channel* channel = new ResourceAnimation::Channel;
		std::string channelName;

		unsigned int nodeHeader[3];
		bytes = sizeof(nodeHeader);
		memcpy(nodeHeader, fileBuffer, bytes);
		fileBuffer += bytes;

		char* name = new char[nodeHeader[0]]{};
		bytes = sizeof(char) * (unsigned int)nodeHeader[0];
		memcpy(name, fileBuffer, bytes);
		fileBuffer += bytes;
		channelName = std::string(name, nodeHeader[0]);
		delete[] name;

		float3* positionsPointer = new float3[nodeHeader[1]];
		bytes = sizeof(float3) * nodeHeader[1];
		memcpy(positionsPointer, fileBuffer, bytes);
		fileBuffer += bytes;
		std::vector<float3> positions(positionsPointer, positionsPointer + nodeHeader[1]);
		channel->positions = positions;
		delete[] positionsPointer;

		Quat* rotationsPointer = new Quat[nodeHeader[2]];
		bytes = sizeof(Quat) * nodeHeader[2];
		memcpy(rotationsPointer, fileBuffer, bytes);
		fileBuffer += bytes;
		std::vector<Quat> rotations(rotationsPointer, rotationsPointer + nodeHeader[2]);
		channel->rotations = rotations;
		delete[] rotationsPointer;

		channels[channelName] = channel;
	}
	resource->SetChannels(channels);
}

void AnimationImporter::Save(const std::shared_ptr<ResourceAnimation>& resource, char*& fileBuffer, unsigned int& size)
{
	unsigned int header[1] =
	{
		resource->GetNumChannels()
	};

	size = (sizeof(unsigned int) * 3) * resource->GetNumChannels() + sizeof(header) + sizeof(double);

	for (auto it: resource->GetChannels())
	{
		ResourceAnimation::Channel* channel = it.second;
		size += sizeof(float3) * channel->positions.size();
		size += sizeof(Quat) * channel->rotations.size();
		size += sizeof(char)* it.first.length();
	}

	char* cursor = new char[size] {};

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	bytes = sizeof(double);

	double duration = resource->GetDuration();
	memcpy(cursor, &duration, bytes);

	cursor += bytes;

	for (auto it : resource->GetChannels())
	{
		ResourceAnimation::Channel* channel = it.second;

		unsigned int nodeHeader[3] =
		{
			it.first.length(),
			channel->positions.size(),
			channel->rotations.size(),
		};

		bytes = sizeof(nodeHeader);
		memcpy(cursor, nodeHeader, bytes);

		cursor += bytes;

		bytes = sizeof(char) * it.first.length();
		memcpy(cursor, it.first.c_str(), bytes);

		cursor += bytes;

		if (!channel->positions.empty())
		{
			bytes = sizeof(float3) * channel->positions.size();
			memcpy(cursor, &(channel->positions[0]), bytes);

			cursor += bytes;
		}

		if (!channel->rotations.empty())
		{
			bytes = sizeof(Quat) * channel->rotations.size();
			memcpy(cursor, &(channel->rotations[0]), bytes);

			cursor += bytes;
		}
	}
}
