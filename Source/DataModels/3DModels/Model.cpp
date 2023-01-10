#include "Model.h"

#include "Globals.h"
#include "Application.h"

#include "ModuleTexture.h"
#include "ModuleEngineCamera.h"
#include "ModuleScene.h"

#include "Mesh.h"

#include "Math/Quat.h"

#include "assimp/scene.h"     
#include "assimp/postprocess.h"
#include "assimp/cimport.h"
#include "assimp/types.h"
#include "assimp/material.h"


void myCallback(const char* msg, char* userData) {
	ENGINE_LOG("[assimp]%s", msg);
}

Model::Model()
{
	struct aiLogStream stream;
	stream.callback = myCallback;
	aiAttachLogStream(&stream);

	aabb.SetNegativeInfinity();
	
	translation = float3(0.0f, 0.0f, 0.0f);
	rotation = float3(0.0f, 0.0f, 0.0f);
	scale = float3(1.0f, 1.0f, 1.0f);
}

Model::~Model()
{
	ENGINE_LOG("Destroying model");

	meshes.clear();

	for (unsigned texture : textures)
	{
		glDeleteTextures(1, &texture);
	}

	textures.clear();
}

void Model::Load(const char* fileName)
{
	this->path = fileName;

	ENGINE_LOG("Model: %s", fileName);

	const aiScene* scene = aiImportFile(fileName, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene)
	{
		LoadMaterials(scene);
		LoadMeshes(scene);

		App->engineCamera->Focus(this->aabb);
	}
	else
	{
		ENGINE_LOG("Error loading %s: %s", fileName, aiGetErrorString());
	}
}

void Model::LoadMaterials(const aiScene* scene)
{
	aiString file;

	textures.reserve(scene->mNumMaterials);

	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS)
		{
			unsigned int textWidth, textHeight;
			textures.push_back(App->textures->Load(file.data, &GetDirectory()[0], textWidth, textHeight));
			textureWidths.push_back(textWidth);
			textureHeights.push_back(textHeight);
		}
	}
}

void Model::LoadMeshes(const aiScene* scene)
{
	meshes.reserve(scene->mNumMeshes);

	for (unsigned i = 0; i < scene->mNumMeshes; ++i)
	{
		Mesh* mesh = new Mesh(scene->mMeshes[i]);

		aabb.Enclose(mesh->GetVertices(), mesh->GetNumVertices());

		meshes.push_back(std::unique_ptr<Mesh>(mesh));
	}

	obb = aabb.Transform(float4x4::FromTRS(translation, GetRotationF4x4(), scale));
}

void Model::Draw()
{
	if (App->engineCamera->IsInside(obb) || App->scene->IsInsideACamera(obb))
	{
		for (int i = 0; i < meshes.size(); ++i)
		{
			meshes[i]->Draw(textures, translation, GetRotationF4x4(), scale);
		}
	}
}

const std::string Model::GetDirectory() const
{
	std::string directory = this->path;
	bool found = false;

	int i = directory.size() - 1;
	for (; !found && i > 0; --i)
	{
		if (directory[i] == '\\') found = true;
	}

	if (found)
	{
		directory = directory.substr(0, i + 2);
	}

	return directory;
}

int Model::GetNumVertices() const
{
	int count = 0;

	for (int i = 0; i < meshes.size(); ++i)
	{
		count += meshes[i]->GetNumVertices();
	}

	return count;
}

int Model::GetNumTriangles() const
{
	int count = 0;

	for (int i = 0; i < meshes.size(); ++i)
	{
		count += meshes[i]->GetNumTriangles();
	}

	return count;
}

unsigned Model::GetTextureID(unsigned idx) const
{
	if (!textures.empty())
		return textures[idx];
}

const float4x4& Model::GetRotationF4x4() const
{
	Quat&& rotation = Quat(
		Quat(float3::unitX, DegToRad(this->rotation.x)) *
		Quat(float3::unitY, DegToRad(this->rotation.y)) *
		Quat(float3::unitZ, DegToRad(this->rotation.z))
	);

	return float4x4::FromQuat(rotation);
}

void Model::SetScale(const float3& scale)
{
	if (scale.x == 0 || scale.y == 0 || scale.z == 0)return;
	aabb.Scale(aabb.Centroid(), float3(scale.x / this->scale.x, scale.y / this->scale.y, scale.z / this->scale.z));
	this->scale = scale;
	obb = aabb.Transform(float4x4::FromTRS(translation, GetRotationF4x4(), float3::one));
}

void Model::SetRotation(const float3 &rotation)
{
	this->rotation = rotation;
}

void Model::SetTranslation(const float3 &translation)
{
	this->translation = translation;
}
