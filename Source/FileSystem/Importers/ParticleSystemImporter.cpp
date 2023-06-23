#include "ParticleSystemImporter.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/Json.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "Resources/ResourceTexture.h"
#include "ParticleSystem/ParticleModule.h"

#include "ParticleSystem/ModuleBase.h"
#include "ParticleSystem/ModuleSpawn.h"
#include "ParticleSystem/ModuleRenderer.h"
#include "ParticleSystem/ModuleColor.h"

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

	Json jsonTextures = meta["TexturesAssetPaths"];
	Json jsonVisibleConfig = meta["ConfigEmitters"];
	
	unsigned int countParticleTextures = 0;
#endif

	unsigned int header[1] =
	{
		resource->GetNumEmitters()
	};

	size = sizeof(header)
		// header of emitter (Size name + size Modules + check Resource) + content of Emitter
		+ (sizeof(unsigned int) * 5 + sizeof(float) * 17 + sizeof(bool) * 6) * resource->GetNumEmitters();

	for(size_t i = 0; i < header[0]; i++)
	{
		const ParticleEmitter* emitter = resource->GetEmitter(i);
		size += emitter->GetName().size();
		if(emitter->GetParticleTexture() != nullptr)
		{
			size += sizeof(UID);
		}
		size += (sizeof(int) + sizeof(bool)) * emitter->GetModules().size();

		for (ParticleModule* module : emitter->GetModules())
		{
			switch (module->GetType())
			{
				case ParticleModule::ModuleType::BASE:
					size += sizeof(float) * 7;
					break;
				case ParticleModule::ModuleType::SPAWN:
					size += sizeof(float);
					break;
				case ParticleModule::ModuleType::RENDER:
					//size += sizeof(int) * 2;
					break;
				case ParticleModule::ModuleType::POSITION:
					break;
				case ParticleModule::ModuleType::COLOR:
					//size += sizeof(ImGradientMark) * 2; //It's a struct so it's continuous memory
					break;
				default:
					break;
			}
		}

	}

	char* cursor = new char[size];

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	for (size_t i = 0; i < header[0]; i++)
	{
		const ParticleEmitter* emitter = resource->GetEmitter(i);

		unsigned int emitterHeader[3] =
		{
			emitter->GetName().size(),
			emitter->GetModules().size(),
			emitter->GetParticleTexture() != nullptr ? true : false
		};

		bytes = sizeof(emitterHeader);
		memcpy(cursor, emitterHeader, bytes);

		cursor += bytes;

		bytes = sizeof(char) * emitterHeader[0];
		memcpy(cursor, &(emitter->GetName()[0]), bytes);

		cursor += bytes;

		for (ParticleModule* module : emitter->GetModules())
		{
			SaveModule(cursor, module);
		}

		if(emitterHeader[2])
		{
#ifdef ENGINE
			jsonTextures[countParticleTextures] = emitter->GetParticleTexture()->GetAssetsPath().c_str();
			++countParticleTextures;
#endif // ENGINE

			bytes = sizeof(UID);
			UID textureUID = emitter->GetParticleTexture()->GetUID();
			memcpy(cursor, &textureUID, bytes);

			cursor += bytes;
		}

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
		bool looping = emitter->IsLooping();
		memcpy(cursor, &looping, bytes);

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

#ifdef ENGINE
		jsonVisibleConfig[i] = static_cast<bool>(emitter->IsVisibleConfig());
#endif // ENGINE
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
#ifdef ENGINE
	//Update Meta
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	App->GetModule<ModuleFileSystem>()->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;

	Json jsonTextures = meta["TexturesAssetPaths"];
	Json jsonVisibleConfig = meta["ConfigEmitters"];
	unsigned int countParticleTextures = 0;
#endif

	unsigned int header[1];
	unsigned int bytes = sizeof(header);
	memcpy(header, fileBuffer, bytes);
	fileBuffer += bytes;

	resource->ClearAllEmitters();
	for (unsigned int i = 0; i < header[0]; ++i)
	{
		std::unique_ptr<ParticleEmitter> emitter = std::make_unique<ParticleEmitter>();

		unsigned int emitterHeader[3];
		bytes = sizeof(emitterHeader);
		memcpy(emitterHeader, fileBuffer, bytes);
		fileBuffer += bytes;

		char* name = new char[emitterHeader[0]]{};
		bytes = sizeof(char) * emitterHeader[0];
		memcpy(name, fileBuffer, bytes);
		emitter->SetName(std::string(name, emitterHeader[0]));
		delete[] name;

		fileBuffer += bytes;

		for (int i = 0; i < emitterHeader[1]; ++i)
		{
			LoadModule(fileBuffer, emitter->GetModules()[i]);
		}

		if (emitterHeader[2])
		{
#ifdef ENGINE
			std::string resourcePath = jsonTextures[countParticleTextures];
			std::shared_ptr<ResourceTexture> resourceParticleTexture = App->GetModule<ModuleResources>()->RequestResource<ResourceTexture>(resourcePath);

			emitter->SetParticleTexture(resourceParticleTexture);
			++countParticleTextures;
			fileBuffer += sizeof(UID);
#else
			UID resourcePointer;
			bytes = sizeof(UID);
			memcpy(&resourcePointer, fileBuffer, bytes);
			std::shared_ptr<ResourceTexture> resourceParticleTexture =
				App->GetModule<ModuleResources>()->SearchResource<ResourceTexture>(resourcePointer);
			emitter->SetParticleTexture(resourceParticleTexture);
			fileBuffer += bytes;
#endif
		}

		bytes = sizeof(int);
		int maxParticles;
		memcpy(&maxParticles, fileBuffer, bytes);
		emitter->SetMaxParticles(maxParticles);
		fileBuffer += bytes;

		bytes = sizeof(float);
		float duration;
		memcpy(&duration, fileBuffer, bytes);
		emitter->SetDuration(duration);
		fileBuffer += bytes;

		bytes = sizeof(float) * 2;
		float2 lifeSpan;
		memcpy(&lifeSpan, fileBuffer, bytes);
		emitter->SetLifespanRange(lifeSpan);
		fileBuffer += bytes;

		bytes = sizeof(float) * 2;
		float2 speed;
		memcpy(&speed, fileBuffer, bytes);
		emitter->SetSpeedRange(speed);
		fileBuffer += bytes;

		bytes = sizeof(float) * 2;
		float2 size;
		memcpy(&size, fileBuffer, bytes);
		emitter->SetSizeRange(size);
		fileBuffer += bytes;

		bytes = sizeof(float) * 2;
		float2 rotation;
		memcpy(&rotation, fileBuffer, bytes);
		emitter->SetRotationRange(rotation);
		fileBuffer += bytes;

		bytes = sizeof(float) * 2;
		float2 gravity;
		memcpy(&gravity, fileBuffer, bytes);
		emitter->SetGravityRange(gravity);
		fileBuffer += bytes;

		bytes = sizeof(float) * 4;
		float4 color;
		memcpy(&color, fileBuffer, bytes);
		emitter->SetColor(color);
		fileBuffer += bytes;

		bytes = sizeof(float);
		float angle;
		memcpy(&angle, fileBuffer, bytes);
		emitter->SetAngle(angle);
		fileBuffer += bytes;

		bytes = sizeof(float);
		float radius;
		memcpy(&radius, fileBuffer, bytes);
		emitter->SetRadius(radius);
		fileBuffer += bytes;

		bytes = sizeof(bool);
		bool looping;
		memcpy(&looping, fileBuffer, bytes);
		emitter->SetLooping(looping);
		fileBuffer += bytes;

		bytes = sizeof(bool);
		bool randomLife;
		memcpy(&randomLife, fileBuffer, bytes);
		emitter->SetRandomLife(randomLife);
		fileBuffer += bytes;

		bytes = sizeof(bool);
		bool randomSpeed;
		memcpy(&randomSpeed, fileBuffer, bytes);
		emitter->SetRandomSpeed(randomSpeed);
		fileBuffer += bytes;

		bytes = sizeof(bool);
		bool randomSize;
		memcpy(&randomSize, fileBuffer, bytes);
		emitter->SetRandomSize(randomSize);
		fileBuffer += bytes;

		bytes = sizeof(bool);
		bool randomRot;
		memcpy(&randomRot, fileBuffer, bytes);
		emitter->SetRandomRotation(randomRot);
		fileBuffer += bytes;

		bytes = sizeof(bool);
		bool randomGrav;
		memcpy(&randomGrav, fileBuffer, bytes);
		emitter->SetRandomGravity(randomGrav);
		fileBuffer += bytes;

		bytes = sizeof(int);
		int type;
		memcpy(&type, fileBuffer, bytes);
		emitter->SetShape(static_cast<ParticleEmitter::ShapeType>(type));
		fileBuffer += bytes;

#ifdef ENGINE
		emitter->SetVisibleConfig(static_cast<bool>(jsonVisibleConfig[i]));
#endif // ENGINE

		resource->AddEmitter(std::move(emitter));
	}
}

void ParticleSystemImporter::SaveModule(char*& cursor, ParticleModule*& module)
{
	unsigned int bytes = sizeof(int);
	int type = static_cast<int>(module->GetType());
	memcpy(cursor, &type, bytes);

	cursor += bytes;

	bytes = sizeof(bool);
	bool enabled = module->IsEnabled();
	memcpy(cursor, &enabled, bytes);

	cursor += bytes;

	switch (module->GetType())
	{
		case ParticleModule::ModuleType::BASE:
		{
			ModuleBase* base = dynamic_cast<ModuleBase*>(module);

			bytes = sizeof(float) * 3;
			float3 transform = base->GetOrigin();
			memcpy(cursor, &transform, bytes);

			cursor += bytes;

			bytes = sizeof(float) * 4;
			Quat enabled = base->GetRotation();
			memcpy(cursor, &enabled, bytes);

			cursor += bytes;

			break;
		}
		case ParticleModule::ModuleType::SPAWN:
		{
			ModuleSpawn* spawn = dynamic_cast<ModuleSpawn*>(module);

			bytes = sizeof(float);
			float spawnRate = spawn->GetSpawnRate();
			memcpy(cursor, &spawnRate, bytes);

			cursor += bytes;

			break;
		}
		case ParticleModule::ModuleType::RENDER:
		{
			/*ModuleRenderer* render = dynamic_cast<ModuleRenderer*>(module);

			bytes = sizeof(int);
			int aligment = static_cast<int>(render->GetAlignment());
			memcpy(cursor, &aligment, bytes);

			cursor += bytes;

			bytes = sizeof(int);
			int blending = static_cast<int>(render->GetBlending());
			memcpy(cursor, &blending, bytes);

			cursor += bytes;

			std::shared_ptr<ResourceTexture> resource = render->GetTexture();

			bytes = sizeof(bool);
			bool checkTexture = resource != nullptr;
			memcpy(cursor, &checkTexture, bytes);

			cursor += bytes;

			if (checkTexture)
			{
				bytes = sizeof(UID);
				UID textureUID = resource->GetUID();
				memcpy(cursor, &textureUID, bytes);

				cursor += bytes;
			}*/

			break;
		}
		case ParticleModule::ModuleType::POSITION:
			break;
		case ParticleModule::ModuleType::COLOR:
		{
			/*ModuleColor* color = dynamic_cast<ModuleColor*>(module);

			bytes = sizeof(ImGradientMark);
			ImGradientMark* firstMark = color->GetGradient().getMarks().front();
			memcpy(cursor, firstMark, bytes);

			cursor += bytes;

			bytes = sizeof(ImGradientMark);
			ImGradientMark* secondMark = color->GetGradient().getMarks().back();
			memcpy(cursor, secondMark, bytes);

			cursor += bytes;

			break;*/
		}
		default:
			break;
	}
}

void ParticleSystemImporter::LoadModule(const char*& fileBuffer, ParticleModule*& module)
{
	unsigned int bytes = sizeof(int);
	int type;
	memcpy(&type, fileBuffer, bytes);

	fileBuffer += bytes;

	bytes = sizeof(bool);
	bool enabled;
	memcpy(&enabled, fileBuffer, bytes);

	fileBuffer += bytes;

	module->SetEnabled(enabled);

	switch (module->GetType())
	{
		case ParticleModule::ModuleType::BASE:
		{
			ModuleBase* base = dynamic_cast<ModuleBase*>(module);

			bytes = sizeof(float) * 3;
			float3 transform;
			memcpy(&transform, fileBuffer, bytes);

			base->SetOrigin(transform);

			fileBuffer += bytes;

			bytes = sizeof(float) * 4;
			Quat rotation;
			memcpy(&rotation, fileBuffer, bytes);

			base->SetRotation(rotation);

			fileBuffer += bytes;

			break;
		}
		case ParticleModule::ModuleType::SPAWN:
		{
			ModuleSpawn* spawn = dynamic_cast<ModuleSpawn*>(module);

			bytes = sizeof(float);
			float spawnRate;
			memcpy(&spawnRate, fileBuffer, bytes);

			spawn->SetSpawnRate(spawnRate);

			fileBuffer += bytes;

			break;
		}
		case ParticleModule::ModuleType::RENDER:
		{
			/*ModuleRenderer* render = dynamic_cast<ModuleRenderer*>(module);

			bytes = sizeof(int);
			int alignment;
			memcpy(&alignment, fileBuffer, bytes);

			render->SetAlignment(static_cast<ModuleRenderer::Alignment>(alignment));

			fileBuffer += bytes;

			bytes = sizeof(int);
			int blending;
			memcpy(&blending, fileBuffer, bytes);

			render->SetBlending(static_cast<ModuleRenderer::BlendingMode>(blending));

			fileBuffer += bytes;*/

			break;
		}
		case ParticleModule::ModuleType::POSITION:
			break;
		case ParticleModule::ModuleType::COLOR:
		{
			/*ModuleColor* color = dynamic_cast<ModuleColor*>(module);

			ImGradient colorGradient;

			bytes = sizeof(ImGradientMark);
			ImGradientMark* firstMark = new ImGradientMark;
			memcpy(firstMark, fileBuffer, bytes);

			colorGradient.addMark(firstMark->position,
				ImColor(firstMark->color[0], firstMark->color[1], firstMark->color[2], firstMark->color[3]));
			delete firstMark;
			fileBuffer += bytes;

			bytes = sizeof(ImGradientMark);
			ImGradientMark* secondMark = new ImGradientMark;
			memcpy(secondMark, fileBuffer, bytes);

			colorGradient.addMark(
				secondMark->position,
				ImColor(secondMark->color[0], secondMark->color[1], secondMark->color[2], secondMark->color[3]));
			delete secondMark;
			fileBuffer += bytes;
			
			color->SetGradient(colorGradient);
			break;*/
		}
		default:
			break;
	}
}