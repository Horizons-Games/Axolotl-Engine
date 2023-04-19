#pragma warning (disable: 6386)

#include "StateMachineImporter.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/Json.h"

StateMachineImporter::StateMachineImporter()
{
}

StateMachineImporter::~StateMachineImporter()
{
}

void StateMachineImporter::Import(const char* filePath, std::shared_ptr<ResourceStateMachine> resource)
{
	char* saveBuffer{};
	unsigned int size;
	Save(resource, saveBuffer, size);
	App->fileSystem->Save((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), saveBuffer, size);
}

void StateMachineImporter::Save(const std::shared_ptr<ResourceStateMachine>& resource, char*& fileBuffer, unsigned int& size)
{
#ifdef ENGINE
	//Update Meta
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	App->fileSystem->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;

	Json jsonResources = meta["ResourcesAssetPaths"];
	unsigned int countResources = 0;
#endif

	unsigned int header[2] =
	{
		resource->GetNumStates(),
		resource->GetNumTransitions()
	};

	size = sizeof(header)
		+ (sizeof(unsigned int) + sizeof(UID)) * resource->GetNumStates()
		+ (sizeof(unsigned int) * 2 + sizeof(double)) * resource->GetNumTransitions();
	
	for(const State* state : resource->GetStates())
	{
		size += state->name.size();
	}

	char* cursor = new char[size];

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	for(const State* state : resource->GetStates())
	{
		unsigned int stateHeader[2] =
		{
			state->name.size(),
			state->resource != nullptr ? true : false
		};
		
		bytes = sizeof(stateHeader);
		memcpy(cursor, stateHeader, bytes);

		cursor += bytes;

		bytes = sizeof(char) * stateHeader[0];
		memcpy(cursor, &(state->name[0]), bytes);

		cursor += bytes;

		if(state->resource != nullptr)
		{
#ifdef ENGINE
			jsonResources[countResources] = state->resource->GetAssetsPath().c_str();
			++countResources;
#endif

			bytes = sizeof(UID);
			UID resourceOfStateUID = state->resource->GetUID();
			memcpy(cursor, &resourceOfStateUID, bytes);

			cursor += bytes;
		}
	}

	for (const auto& transitionIterator : resource->GetTransitions())
	{
		int transitionStatesID[2] =
		{
			resource->GetIdState(*transitionIterator.second.origin),
			resource->GetIdState(*transitionIterator.second.destination)
		};
		
		bytes = sizeof(transitionStatesID);
		memcpy(cursor, transitionStatesID, bytes);

		cursor += bytes;

		bytes = sizeof(double);
		memcpy(cursor, &transitionIterator.second.transitionDuration, bytes);

		cursor += bytes;
	}

#ifdef ENGINE
	rapidjson::StringBuffer buffer;
	meta.toBuffer(buffer);
	App->fileSystem->Save(metaPath.c_str(), buffer.GetString(), (unsigned int)buffer.GetSize());
#endif
}

void StateMachineImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceStateMachine> resource)
{
#ifdef ENGINE
	//Update Meta
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	App->fileSystem->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;

	Json jsonResources = meta["ResourcesAssetPaths"];
	unsigned int countResources = 0;
#endif

	unsigned int header[2];
	unsigned int bytes = sizeof(header);
	memcpy(header, fileBuffer, bytes);

	fileBuffer += bytes;

	std::vector<State> states;
	states.reserve(header[0]);
	for(unsigned int i = 0; i < header[0]; i++)
	{
		State state;
		unsigned int stateHeader[2];
		bytes = sizeof(stateHeader);
		memcpy(stateHeader, fileBuffer, bytes);

		fileBuffer += bytes;

		char* name = new char[stateHeader[0]]{};
		bytes = sizeof(char) * (unsigned int)stateHeader[0];
		memcpy(name, fileBuffer, bytes);
		state.name = std::string(name, stateHeader[0]);
		delete[] name;

		fileBuffer += bytes;
		
		if(stateHeader[1])
		{
#ifdef ENGINE
			std::string resourcePath = jsonResources[countResources];
			std::shared_ptr<Resource> resource = App->resources->RequestResource<Resource>(resourcePath);

			state.resource = resource;
			++countResources;

			fileBuffer += sizeof(UID);
#else
			UID resourcePointer;
			bytes = sizeof(UID);
			memcpy(&resourcePointer, fileBuffer, bytes);
			std::shared_ptr<Resource> mesh = App->resources->SearchResource<Resource>(resourcePointer);
			state.resource = resource;
			fileBuffer += bytes;
#endif
		}
	}

}