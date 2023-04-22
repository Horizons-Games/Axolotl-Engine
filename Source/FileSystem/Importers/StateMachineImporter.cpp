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

	unsigned int header[3] =
	{
		resource->GetNumStates(),
		resource->GetNumTransitions(),
		resource->GetNumParameters()
	};

	size = sizeof(header)
		//size of 3 vectors + check resource + size of our own UID + size of int pair Pos (This last maybe on meta?)
		+ (sizeof(unsigned int) * 4 + sizeof(UID) + sizeof(int) * 2) * resource->GetNumStates();
		// size of name vector + enum
		+(sizeof(unsigned int) * 2) * resource->GetNumParameters();
		// size of 2 pos State + Own UID Key + double
		+ (sizeof(unsigned int) * 2 + sizeof(UID) + sizeof(double)) * resource->GetNumTransitions();
	
	for(const State* state : resource->GetStates())
	{
		size += sizeof(char) * state->name.size();
		if(state->resource != nullptr) size += sizeof(UID);
		size += sizeof(UID) * state->transitionsOriginedHere.size();
		size += sizeof(UID) * state->transitionsDestinedHere.size();
	}

	for(const auto& it : resource->GetTransitions())
	{
		//Size of nameSize and Enum Condition
		size += (sizeof(unsigned int) * 2) * it.second.conditions.size();

		for (const Condition& condition : it.second.conditions) 
		{
			size += sizeof(char) * condition.parameter.size();
			switch (resource->GetParameters().find(condition.parameter)->second.first)
			{
			case FieldType::FLOAT:
				size += sizeof(float);
				break;
			case FieldType::BOOL:
				size += sizeof(bool);
				break;
			}
		}
	}

	for (const auto& it : resource->GetParameters())
	{
		size += sizeof(char) * it.first.size();
		switch (it.second.first)
		{
		case FieldType::FLOAT:
			size += sizeof(float);
			break;
		case FieldType::BOOL:
			size += sizeof(bool);
			break;
		}
	}

	char* cursor = new char[size];

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	for(const State* state : resource->GetStates())
	{
		unsigned int stateHeader[4] =
		{
			state->name.size(),
			state->resource != nullptr ? true : false,
			state->transitionsOriginedHere.size(),
			state->transitionsDestinedHere.size()
		};
		
		bytes = sizeof(stateHeader);
		memcpy(cursor, stateHeader, bytes);

		cursor += bytes;

		bytes = sizeof(UID);
		memcpy(cursor, &(state->id), bytes);

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

		bytes = sizeof(int);
		memcpy(cursor, &(state->auxiliarPos.first), bytes);
		cursor += bytes;
		memcpy(cursor, &(state->auxiliarPos.second), bytes);

		cursor += bytes;

		bytes = sizeof(UID) * state->transitionsOriginedHere.size();
		memcpy(cursor, &(state->transitionsOriginedHere[0]),bytes);

		cursor += bytes;

		bytes = sizeof(UID) * state->transitionsDestinedHere.size();
		memcpy(cursor, &(state->transitionsDestinedHere[0]), bytes);

		cursor += bytes;
	}

	for (const auto& parameterIterator : resource->GetParameters())
	{
		unsigned int parameterHeader[2] =
		{
			parameterIterator.first.size(),
			static_cast<unsigned int>(parameterIterator.second.first)
		};

		bytes = sizeof(parameterHeader);
		memcpy(cursor, &parameterHeader, bytes);

		cursor += bytes;

		bytes = sizeof(char) * parameterHeader[0];
		memcpy(cursor, &(parameterIterator.first[0]), bytes);

		cursor += bytes;

		switch (parameterIterator.second.first)
		{
		case FieldType::FLOAT:
			bytes = sizeof(float);
			break;
		case FieldType::BOOL:
			bytes = sizeof(bool);
			break;
		default:
			break;
		}
		memcpy(cursor, &(parameterIterator.second.second), bytes);
		cursor += bytes;
	}

	for (const auto& transitionIterator : resource->GetTransitions())
	{
		unsigned int transitionHeader[3] =
		{
			resource->GetIdState(*transitionIterator.second.origin),
			resource->GetIdState(*transitionIterator.second.destination),
			transitionIterator.second.conditions.size()
		};
		
		bytes = sizeof(transitionHeader);
		memcpy(cursor, transitionHeader, bytes);

		cursor += bytes;

		bytes = sizeof(double);
		memcpy(cursor, &(transitionIterator.second.transitionDuration), bytes);

		cursor += bytes;

		for(const Condition& condition : transitionIterator.second.conditions)
		{
			unsigned int conditionHeader[2] =
			{
				condition.parameter.size(),
				static_cast<unsigned int>(condition.conditionType)
			};

			bytes = sizeof(conditionHeader);
			memcpy(cursor, conditionHeader, bytes);

			cursor += bytes;

			bytes = sizeof(char) * conditionHeader[0];
			memcpy(cursor, &(condition.parameter[0]), bytes);

			cursor += bytes;

			switch (resource->GetParameters().find(condition.parameter)->second.first)
			{
			case FieldType::FLOAT:
				bytes = sizeof(float);
				break;
			case FieldType::BOOL:
				bytes = sizeof(bool);
				break;
			default:
				break;
			}
			memcpy(cursor, &(condition.value), bytes);
			cursor += bytes;
		}
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