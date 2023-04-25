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
	char* loadBuffer{};
	App->fileSystem->Load(filePath, loadBuffer);
	Load(loadBuffer, resource);

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
		resource->GetNumParameters(),
		resource->GetNumTransitions()
	};

	size = sizeof(header)
		//size of 3 vectors + check resource + size of our own UID + size of int pair Pos (This last maybe on meta?)
		+ (sizeof(unsigned int) * 4 + sizeof(UID) + sizeof(int) * 2) * resource->GetNumStates()
		// size of name vector + enum
		+ (sizeof(unsigned int) * 2) * resource->GetNumParameters()
		// size of 2 pos State + size vector conditions + Own UID Key + double
		+ (sizeof(unsigned int) * 3 + sizeof(UID) + sizeof(double)) * resource->GetNumTransitions();
	
	for(const State* state : resource->GetStates())
	{
		size += sizeof(char) * state->name.size();
		if(state->resource != nullptr) size += sizeof(UID);
		size += sizeof(UID) * state->transitionsOriginedHere.size();
		size += sizeof(UID) * state->transitionsDestinedHere.size();
	}

	for (const auto& it : resource->GetParameters())
	{
		size += sizeof(char) * it.first.size();
		switch (it.second.first)
		{
		case FieldTypeParameter::FLOAT:
			size += sizeof(float);
			break;
		case FieldTypeParameter::BOOL:
			size += sizeof(bool);
			break;
		}
	}

	for(const auto& it : resource->GetTransitions())
	{
		//Size of nameSize and Enum Condition
		size += (sizeof(unsigned int) * 2) * it.second.conditions.size();

		for (const Condition& condition : it.second.conditions)
		{
			size += sizeof(char) * condition.parameter.size();
			const auto& itParameter = resource->GetParameters().find(condition.parameter);
			if(itParameter != resource->GetParameters().end())
			{
				switch (itParameter->second.first)
				{
				case FieldTypeParameter::FLOAT:
					size += sizeof(float);
					break;
				case FieldTypeParameter::BOOL:
					//Bool Condition doesn't have a value
					break;
				}
			}
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
		if(!state->transitionsOriginedHere.empty()) memcpy(cursor, &(state->transitionsOriginedHere[0]),bytes);

		cursor += bytes;

		bytes = sizeof(UID) * state->transitionsDestinedHere.size();
		if (!state->transitionsDestinedHere.empty())memcpy(cursor, &(state->transitionsDestinedHere[0]), bytes);

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

		if(parameterIterator.second.first == FieldTypeParameter::FLOAT)
		{
			bytes = sizeof(float);
			float auxFloat = std::get<float>(parameterIterator.second.second);
			memcpy(cursor, &auxFloat, bytes);
		}
		else if (parameterIterator.second.first == FieldTypeParameter::BOOL)
		{
			bytes = sizeof(bool);
			bool auxBool = std::get<bool>(parameterIterator.second.second);
			memcpy(cursor, &auxBool, bytes);
		}

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

		bytes = sizeof(UID);
		memcpy(cursor, &(transitionIterator.first), bytes);

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

			const auto& itParameter = resource->GetParameters().find(condition.parameter);
			if(itParameter != resource->GetParameters().end())
			{
				if (itParameter->second.first == FieldTypeParameter::FLOAT)
				{
					bytes = sizeof(float);
					float auxFloat = std::get<float>(condition.value);
					memcpy(cursor, &auxFloat, bytes);
					cursor += bytes;
				}
				else if (itParameter->second.first == FieldTypeParameter::BOOL)
				{
					//Condition bool doesn't have value
				}
			}
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

	unsigned int header[3];
	unsigned int bytes = sizeof(header);
	memcpy(header, fileBuffer, bytes);

	fileBuffer += bytes;

	for(unsigned int i = 0; i < header[0]; i++)
	{
		std::unique_ptr<State> state = std::make_unique<State>();

		unsigned int stateHeader[4];
		bytes = sizeof(stateHeader);
		memcpy(stateHeader, fileBuffer, bytes);

		fileBuffer += bytes;

		bytes = sizeof(UID);
		memcpy(&(state->id), fileBuffer, bytes);

		fileBuffer += bytes;

		char* name = new char[stateHeader[0]]{};
		bytes = sizeof(char) * stateHeader[0];
		memcpy(name, fileBuffer, bytes);
		state->name = std::string(name, stateHeader[0]);
		delete[] name;

		fileBuffer += bytes;
		
		if(stateHeader[1])
		{
#ifdef ENGINE
			std::string resourcePath = jsonResources[countResources];
			std::shared_ptr<Resource> resource = App->resources->RequestResource<Resource>(resourcePath);

			state->resource = resource;
			++countResources;

			fileBuffer += sizeof(UID);
#else
			UID resourcePointer;
			bytes = sizeof(UID);
			memcpy(&resourcePointer, fileBuffer, bytes);
			std::shared_ptr<Resource> mesh = App->resources->SearchResource<Resource>(resourcePointer);
			state->resource = resource;
			fileBuffer += bytes;
#endif
		}

		int firstPos;
		int secondPos;
		bytes = sizeof(int);
		memcpy(&firstPos, fileBuffer, bytes);

		fileBuffer += bytes;

		memcpy(&secondPos, fileBuffer, bytes);
		state->auxiliarPos = std::make_pair(firstPos, secondPos);

		fileBuffer += bytes;

		UID* transitionsOriginedPointer = new UID[stateHeader[2]];
		bytes = sizeof(UID) * stateHeader[2];
		if(stateHeader[2] > 0)
		{
			memcpy(transitionsOriginedPointer, fileBuffer, bytes);
			std::vector<UID> transitionsOrigined = std::vector(transitionsOriginedPointer, transitionsOriginedPointer + stateHeader[2]);
			state->transitionsOriginedHere = transitionsOrigined;
		}
		delete[] transitionsOriginedPointer;

		fileBuffer += bytes;

		UID* transitionsDestinedPointer = new UID[stateHeader[3]];
		bytes = sizeof(UID) * stateHeader[3];
		if(stateHeader[3] > 0) 
		{
			memcpy(transitionsDestinedPointer, fileBuffer, bytes);
			std::vector<UID> transitionsDestined = std::vector(transitionsDestinedPointer, transitionsDestinedPointer + stateHeader[3]);
			state->transitionsDestinedHere = transitionsDestined;
		}
		delete[] transitionsDestinedPointer;

		fileBuffer += bytes;

		resource->AddState(std::move(state));
	}

	std::unordered_map<std::string, TypeFieldPairParameter> parameters;
	parameters.reserve(header[1]);
	for(unsigned int i = 0; i < header[1]; i++)
	{
		std::string name;
		TypeFieldPairParameter parameter;

		unsigned int parameterHeader[2];
		bytes = sizeof(parameterHeader);
		memcpy(parameterHeader, fileBuffer, bytes);
		parameter.first = static_cast<FieldTypeParameter>(parameterHeader[1]);

		fileBuffer += bytes;

		char* namePointer = new char[parameterHeader[0]]{};
		bytes = sizeof(char) * parameterHeader[0];
		memcpy(namePointer, fileBuffer, bytes);
		name = std::string(namePointer, parameterHeader[0]);
		delete[] namePointer;

		fileBuffer += bytes;

		if(parameter.first == FieldTypeParameter::FLOAT)
		{
			bytes = sizeof(float);
			float value;
			memcpy(&value, fileBuffer, bytes);
			parameter.second = value;
		}
		else if(parameter.first == FieldTypeParameter::BOOL)
		{
			bytes = sizeof(bool);
			bool value;
			memcpy(&value, fileBuffer, bytes);
			parameter.second = value;
		}
		fileBuffer += bytes;

		parameters[name] = parameter;
	}

	resource->SetMapParameters(parameters);

	std::unordered_map<UID, Transition> transitions;
	transitions.reserve(header[2]);
	for(unsigned int i = 0; i < header[2]; i++)
	{
		Transition transition;
		UID uidTransition;

		unsigned int transitionHeader[3];
		bytes = sizeof(transitionHeader);
		memcpy(transitionHeader, fileBuffer, bytes);
		transition.origin = resource->GetStates()[transitionHeader[0]];
		transition.destination = resource->GetStates()[transitionHeader[1]];

		fileBuffer += bytes;

		bytes = sizeof(UID);
		memcpy(&uidTransition,fileBuffer,bytes);

		fileBuffer += bytes;

		bytes = sizeof(double);
		memcpy(&(transition.transitionDuration), fileBuffer, bytes);

		fileBuffer += bytes;

		std::vector<Condition> conditions;
		conditions.reserve(transitionHeader[2]);
		for (int j = 0; j < transitionHeader[2]; j++)
		{
			Condition condition;

			unsigned int conditionHeader[2];
			bytes = sizeof(conditionHeader);
			memcpy(conditionHeader, fileBuffer, bytes);
			condition.conditionType = static_cast<ConditionType>(conditionHeader[1]);
			
			fileBuffer += bytes;

			char* namePointer = new char[conditionHeader[0]]{};
			bytes = sizeof(char) * conditionHeader[0];
			memcpy(namePointer, fileBuffer, bytes);
			condition.parameter = std::string(namePointer, conditionHeader[0]);
			delete[] namePointer;
			
			fileBuffer += bytes;

			const auto& itParameter = resource->GetParameters().find(condition.parameter);
			if(itParameter != resource->GetParameters().end())
			{
				if (itParameter->second.first == FieldTypeParameter::FLOAT)
				{
					bytes = sizeof(float);
					float value;
					memcpy(&value, fileBuffer, bytes);
					condition.value = value;
					fileBuffer += bytes;
				}
				else if (itParameter->second.first == FieldTypeParameter::BOOL)
				{
					//Condition Bool doesn't have value
				}
			}

			conditions.push_back(condition);
		}

		transition.conditions = conditions;
		transitions[uidTransition] = transition;
	}

	resource->SetTransitions(transitions);
}