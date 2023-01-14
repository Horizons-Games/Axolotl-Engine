#include "Model.h"

#include "Globals.h"
#include "Application.h"

#include "ModuleTexture.h"
#include "ModuleEngineCamera.h"
#include "Scene.h"
#include "ModuleScene.h"

#include "Mesh.h"

#include "Math/Quat.h"

#include "assimp/scene.h"     
#include "assimp/postprocess.h"
#include "assimp/cimport.h"
#include "assimp/types.h"
#include "assimp/material.h"

#include "DataModels/Resources/ResourceMaterial.h"
#include "DataModels/Resources/ResourceTexture.h"
#include "DataModels/Resources/ResourceMesh.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"


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

	for (int i = 0; i < materials.size(); ++i)
	{
		glDeleteTextures(1, &materials[i]->diffuse);
		glDeleteTextures(1, &materials[i]->normal);
		glDeleteTextures(1, &materials[i]->occlusion);
		glDeleteTextures(1, &materials[i]->specular);
	}

	materials.clear();


	for (unsigned texture : textures)
	{
		glDeleteTextures(1, &texture);
	}

	textures.clear();

}

void Model::SetFromResource(std::shared_ptr<ResourceModel>& resource) //Temporal setter
{
	path = resource->GetAssetsPath().c_str();

	std::vector<UID> materialsUIDs = resource->GetMaterialsUIDs();

	for (int i = 0; i < materialsUIDs.size(); ++i) 
	{
		std::shared_ptr<ResourceMaterial> resourceMaterial = 
			std::dynamic_pointer_cast<ResourceMaterial>(App->resources->RequestResource(materialsUIDs[i]));

		Material* material = new Material();
		resourceMaterial->Load();

		if (resourceMaterial->haveDiffuse())
		{
			std::shared_ptr<ResourceTexture> textureDiffuse =
				std::dynamic_pointer_cast<ResourceTexture>(App->resources->RequestResource(resourceMaterial->GetDiffuseUID()));

			textureDiffuse->Load();
			material->haveDiffuse = true;
			material->diffuse = textureDiffuse->GetGlTexture();
			textureHeights.push_back(textureDiffuse->GetHeight());
			textureWidths.push_back(textureDiffuse->GetWidth());
		}

		if (resourceMaterial->haveNormal())
		{
			std::shared_ptr<ResourceTexture> textureNormal =
				std::dynamic_pointer_cast<ResourceTexture>(App->resources->RequestResource(resourceMaterial->GetNormalUID()));

			textureNormal->Load();
			material->normal = textureNormal->GetGlTexture();
			material->haveNormal = true;
			textureHeights.push_back(textureNormal->GetHeight());
			textureWidths.push_back(textureNormal->GetWidth());
		}

		if (resourceMaterial->haveOcclusion())
		{
			std::shared_ptr<ResourceTexture> textureOcclusion =
				std::dynamic_pointer_cast<ResourceTexture>(App->resources->RequestResource(resourceMaterial->GetOcclusionrUID()));

			textureOcclusion->Load();
			material->occlusion = textureOcclusion->GetGlTexture();
			material->haveOcclusion = true;
			textureHeights.push_back(textureOcclusion->GetHeight());
			textureWidths.push_back(textureOcclusion->GetWidth());
		}

		if (resourceMaterial->haveSpecular())
		{
			std::shared_ptr<ResourceTexture> textureSpecular =
				std::dynamic_pointer_cast<ResourceTexture>(App->resources->RequestResource(resourceMaterial->GetSpecularUID()));

			textureSpecular->Load();
			material->specular = textureSpecular->GetGlTexture();
			material->haveOcclusion = true;
			textureHeights.push_back(textureSpecular->GetHeight());
			textureWidths.push_back(textureSpecular->GetWidth());
		}

		material->diffuseColor = resourceMaterial->GetDiffuseColor();
		material->specularColor = resourceMaterial->GetSpecularColor();
		material->shininess = resourceMaterial->GetShininess();
		material->haveShininessAlpha = resourceMaterial->HaveShininessAlpha();
		material->normalStrength = resourceMaterial->GetNormalStrength();

		materials.push_back((std::unique_ptr<Material>(material)));
	}


	std::vector<UID> meshesUIDs = resource->GetMeshesUIDs();

	for (int i = 0; i < meshesUIDs.size(); ++i) 
	{

		std::shared_ptr<ResourceMesh> resourceMesh = 
			std::dynamic_pointer_cast<ResourceMesh>(App->resources->RequestResource(meshesUIDs[i]));

		resourceMesh->Load();
		Mesh* mesh = new Mesh();
		mesh->SetFromResource(resourceMesh);
		meshes.push_back(std::unique_ptr<Mesh>(mesh));

		aabb.Enclose(mesh->GetVertices(), mesh->GetNumVertices());
	}
	obb = aabb.Transform(float4x4::FromTRS(translation, GetRotationF4x4(), scale));

	App->engineCamera->Focus(this->aabb);
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
		Mesh* mesh = new Mesh();
		mesh->Load(scene->mMeshes[i]);

		aabb.Enclose(mesh->GetVertices(), mesh->GetNumVertices());

		meshes.push_back(std::unique_ptr<Mesh>(mesh));
	}

	obb = aabb.Transform(float4x4::FromTRS(translation, GetRotationF4x4(), scale));
}

void Model::Draw()
{
	if (App->engineCamera->IsInside(obb) || App->scene->GetLoadedScene()->IsInsideACamera(obb))
	{
		for (int i = 0; i < meshes.size(); ++i)
		{
			meshes[i]->Draw(textures, translation, GetRotationF4x4(), scale);
		}
	}
}


void Model::NewDraw()
{
	if (App->engineCamera->IsInside(obb))
	{
		for (int i = 0; i < meshes.size(); ++i)
		{
			meshes[i]->NewDraw(materials[meshes[i]->GetMaterialIndex()], translation, GetRotationF4x4(), scale);
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
