#include "ParticleSystemImporter.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/Json.h"
#include "ParticleSystem/ParticleEmitter.h"

ParticleSystemImporter::ParticleSystemImporter()
{
}

ParticleSystemImporter::~ParticleSystemImporter()
{
}

void ParticleSystemImporter::Import
(const char* filePath, std::shared_ptr<ResourceParticleSystem> resource)
{
	char* loadBuffer{};
	App->GetModule<ModuleFileSystem>()->Load(filePath, loadBuffer);
	Load(loadBuffer, resource);

	char* saveBuffer{};
	unsigned int size;
	Save(resource, saveBuffer, size);
	App->GetModule<ModuleFileSystem>()->Save((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), saveBuffer, size);

	delete loadBuffer;
	delete saveBuffer;
}

void ParticleSystemImporter::Save
(const std::shared_ptr<ResourceParticleSystem>& resource,
	char*& fileBuffer, unsigned int& size)
{
#ifdef ENGINE
	//Update Meta
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	App->GetModule<ModuleFileSystem>()->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;
#endif

	unsigned int header[1] =
	{
		resource->GetNumEmitters()
	};

	size = sizeof(header)
		// header of emitter (Size name + size Modules) + content of Emitter
		+ (sizeof(unsigned int) * 4 + sizeof(float) * 17 + sizeof(bool) * 6) * resource->GetNumEmitters();

	for(size_t i = 0; i < header[0]; i++)
	{
		const ParticleEmitter* emitter = resource->GetEmitter(i);
		size += emitter->GetName().size();
		//size += emitter->GetModules().size();
	}

	char* cursor = new char[size];

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	for (size_t i = 0; i < header[0]; i++)
	{
		const ParticleEmitter* emitter = resource->GetEmitter(i);

		unsigned int emitterHeader[2] =
		{
			emitter->GetName().size(),
			emitter->GetModules().size()
			//emitter->resource != nullptr ? true : false,
		};

		bytes = sizeof(emitterHeader);
		memcpy(cursor, emitterHeader, bytes);

		cursor += bytes;

		bytes = sizeof(char) * emitterHeader[0];
		memcpy(cursor, &(emitter->GetName()[0]), bytes);

		cursor += bytes;

		/* Modules
		bytes = sizeof(char) * emitterHeader[0];
		memcpy(cursor, &(emitter->GetName()[0]), bytes);

		cursor += bytes;*/

		bytes = sizeof(int);
		int maxParticles = emitter->GetMaxParticles();
		memcpy(cursor, &maxParticles, bytes);

		cursor += bytes;

		bytes = sizeof(float);
		float duration = emitter->GetDuration();
		memcpy(cursor, &duration, bytes);

		cursor += bytes;

		bytes = sizeof(float) * 2;
		float2 lifeSpan = emitter->GetLifespanRange();
		memcpy(cursor, &lifeSpan, bytes);

		cursor += bytes;

		bytes = sizeof(float) * 2;
		float2 speed = emitter->GetSpeedRange();
		memcpy(cursor, &speed, bytes);

		cursor += bytes;

		bytes = sizeof(float) * 2;
		float2 size = emitter->GetSizeRange();
		memcpy(cursor, &size, bytes);

		cursor += bytes;

		bytes = sizeof(float) * 2;
		float2 rotation = emitter->GetRotationRange();
		memcpy(cursor, &rotation, bytes);

		cursor += bytes;

		bytes = sizeof(float) * 2;
		float2 gravity = emitter->GetGravityRange();
		memcpy(cursor, &gravity, bytes);

		cursor += bytes;

		bytes = sizeof(float) * 4;
		float4 color = emitter->GetColor();
		memcpy(cursor, &color, bytes);

		cursor += bytes;

		bytes = sizeof(float);
		float angle = emitter->GetAngle();
		memcpy(cursor, &angle, bytes);

		cursor += bytes;
	
		bytes = sizeof(float);
		float radius = emitter->GetRadius();
		memcpy(cursor, &radius, bytes);

		cursor += bytes;

		bytes = sizeof(bool);
		bool randomLife = emitter->IsRandomLife();
		memcpy(cursor, &randomLife, bytes);

		cursor += bytes;

		bytes = sizeof(bool);
		bool randomSpeed = emitter->IsRandomSpeed();
		memcpy(cursor, &randomSpeed, bytes);

		cursor += bytes;

		bytes = sizeof(bool);
		bool randomSize = emitter->IsRandomSize();
		memcpy(cursor, &randomSize, bytes);

		cursor += bytes;

		bytes = sizeof(bool);
		bool randomRot = emitter->IsRandomRot();
		memcpy(cursor, &randomRot, bytes);

		cursor += bytes;

		bytes = sizeof(bool);
		bool randomGrav = emitter->IsRandomGravity();
		memcpy(cursor, &randomGrav, bytes);

		cursor += bytes;

		bytes = sizeof(int);
		int type = static_cast<int>(emitter->GetShape());
		memcpy(cursor, &type, bytes);

		cursor += bytes;

		//This last probably go to the meta because is exclusive of the Engine 
		bytes = sizeof(bool);
		bool visibleConfig = emitter->IsVisibleConfig();
		memcpy(cursor, &visibleConfig, bytes);

		cursor += bytes;
	}

#ifdef ENGINE
	rapidjson::StringBuffer buffer;
	meta.toBuffer(buffer);
	App->GetModule<ModuleFileSystem>()->Save(metaPath.c_str(), buffer.GetString(), (unsigned int)buffer.GetSize());
#endif
}

void ParticleSystemImporter::Load
(const char* fileBuffer, std::shared_ptr<ResourceParticleSystem> resource)
{
	
}
