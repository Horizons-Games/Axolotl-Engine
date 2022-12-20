#include "Model.h"

#include "Globals.h"
#include "Application.h"

#include "ModuleTexture.h"
#include "ModuleEngineCamera.h"

#include "Mesh.h"

#include <MathGeoLib/src/Math/Quat.h>

#include <assimp/scene.h>       
#include <assimp/postprocess.h> 
#include <assimp/cimport.h>
#include <assimp/types.h>
#include <assimp/material.h>


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

	for (Mesh* mesh : meshes)
	{
		delete mesh;
		mesh = nullptr;
	}

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
		// TODO: LoadTextures(scene->mMaterials, scene->mNumMaterials);
		// TODO: LoadMeshes(scene->mMeshes, scene->mNumMeshes);
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
			textures.push_back(App->textures->Load(file.data, &GetDirectory()[0]));
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

		meshes.push_back(mesh);
	}

	obb = aabb.Transform(float4x4::FromTRS(translation, GetRotationF4x4(), scale));
}

void Model::Draw()
{
	for (int i = 0; i < meshes.size(); ++i)
	{
		meshes[i]->Draw(textures, translation, GetRotationF4x4(), scale);
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

const AABB& Model::GetAABB() const
{
	return aabb;
}

const OBB& Model::GetOBB() const
{
	return obb;
}


int Model::GetNumVertices() const
{
	int count = 0;

	for (Mesh* mesh : meshes)
	{
		count += mesh->GetNumVertices();
	}

	return count;
}

int Model::GetNumTriangles() const
{
	int count = 0;

	for (Mesh* mesh : meshes)
	{
		count += mesh->GetNumTriangles();
	}

	return count;
}

unsigned Model::GetTextureId(unsigned idx) const
{
	if (!textures.empty())
		return textures[idx];
}

const float3& Model::GetTranslation() const
{
	return translation;
}

const float3& Model::GetScale() const
{
	return scale;
}

const float3& Model::GetRotationF3() const
{
	return rotation;
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
	this->scale = scale;

	obb = aabb.Transform(float4x4::FromTRS(translation, GetRotationF4x4(), scale));
}

void Model::SetRotation(const float3 &rotation)
{
	this->rotation = rotation;

	obb = aabb.Transform(float4x4::FromTRS(translation, GetRotationF4x4(), scale));
}

void Model::SetTranslation(const float3 &translation)
{
	this->translation = translation;

	obb = aabb.Transform(float4x4::FromTRS(translation, GetRotationF4x4(), scale));
}
