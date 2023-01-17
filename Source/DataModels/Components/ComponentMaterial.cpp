#include "ComponentMaterial.h"

#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEngineCamera.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/Json.h"

#include "GameObject/GameObject.h"

#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceTexture.h"
#include "FileSystem/Importers/TextureImporter.h"

#include "../../FileSystem/UniqueID.h"

#include <GL/glew.h>
#include "imgui.h"

ComponentMaterial::ComponentMaterial(bool active, GameObject* owner)
	: Component(ComponentType::MATERIAL, active, owner, true)
{
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::Update()
{
}

void ComponentMaterial::Draw()
{
	unsigned int program = App->program->GetProgram();

	GLint programInUse;
	glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

	if (program != programInUse)
	{
		glUseProgram(program);
	}

	glUniform3f(glGetUniformLocation(program, "material.diffuse_color"), diffuseColor.x, diffuseColor.y, diffuseColor.z);
	std::shared_ptr<ResourceTexture> texture = textureDiffuse.lock();
	if (texture)
	{
		if (!texture->IsLoaded())
		{
			texture->Load();
		}

		glUniform1i(glGetUniformLocation(program, "material.has_diffuse_map"), 1);
		glUniform1i(glGetUniformLocation(program, "material.diffuse_map"), texture->GetGlTexture());
		glActiveTexture(GL_TEXTURE0 + texture->GetGlTexture());
		glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());

	}
	else
	{
		glUniform1i(glGetUniformLocation(program, "material.has_diffuse_map"), 0);
	}
	texture = textureSpecular.lock();
	if (texture)
	{
		if (!texture->IsLoaded())
		{
			texture->Load();
		}

		glUniform1i(glGetUniformLocation(program, "material.has_specular_map"), 1);
		glUniform1i(glGetUniformLocation(program, "material.specular_map"), texture->GetGlTexture());
		glActiveTexture(GL_TEXTURE0 + texture->GetGlTexture());
		glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
	}
	else
	{
		glUniform1i(glGetUniformLocation(program, "material.has_specular_map"), 0);
	}
	texture = textureNormal.lock();
	if (texture)
	{
		if (!texture->IsLoaded())
		{
			texture->Load();
		}

		glActiveTexture(GL_TEXTURE0 + texture->GetGlTexture());
		glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
		glUniform1i(glGetUniformLocation(program, "material.normal_map"), texture->GetGlTexture());
		glUniform1f(glGetUniformLocation(program, "material.normal_strength"), normalStrength);

		glUniform1i(glGetUniformLocation(program, "material.has_normal_map"), 1);
	}
	else
	{
		glUniform1i(glGetUniformLocation(program, "material.has_normal_map"), 0);
	}
	glUniform3f(glGetUniformLocation(program, "material.specular_color"), specularColor.x, specularColor.y, specularColor.z);
	glUniform1f(glGetUniformLocation(program, "material.shininess"), shininess);
	glUniform1f(glGetUniformLocation(program, "material.shininess_alpha"), hasShininessAlpha);

	//move to main component(?)
	float3 viewPos = App->engineCamera->GetPosition();
	glUniform3f(glGetUniformLocation(program, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
}

void ComponentMaterial::Display()
{
	ImGui::Text("");
	ImGui::Text("MATERIAL");
	ImGui::Text("");
	std::shared_ptr<ResourceTexture> texture = textureDiffuse.lock();

	title = "Select Texture";
	dialogName = "Choose File";
	filters = "Image files (*.png *.gif *.jpg *.jpeg){.png,.gif,.jpg,.jpeg,.dds}*";
	startPath = "./Assets/Textures";

	if (ImGui::CollapsingHeader("MATERIAL", ImGuiTreeNodeFlags_DefaultOpen))
	{
			//WindowImporter
	if (ImGui::Button(title))
	{
		fileDialogImporter.OpenDialog("ChooseFileDlgKey", dialogName, filters, startPath,
			"", 1, nullptr);
	}
	// display
	if (fileDialogImporter.Display("ChooseFileDlgKey"))
	{
		// action if OK
		if (fileDialogImporter.IsOk())
		{
			std::string filePathName = fileDialogImporter.GetFilePathName();
			//textureImporter->Load(filePathName.c_str(), texture2);
			//texture = new ResourceTexture(1, filePathName, filePathName, filePathName);
			texture->SetGlTexture(4);
		}

		// close
		fileDialogImporter.Close();
	}
	//End of Window importer

	char* name = new char[256];
	std::string fileName = texture->GetFileName();
	std::strncpy(name, fileName.c_str(), 256);
	ImGui::InputText("Texture Name", name, 256);

	ImGui::Text("");
	if (texture)
		ImGui::Image((void*)texture->GetGlTexture(), ImVec2(100, 100));

	static float3 colorDiffuse = GetDiffuseColor();
	ImGui::Text("Diffuse Color:"); ImGui::SameLine();
	if (ImGui::ColorEdit3("##Diffuse Color", (float*)&colorDiffuse))
		SetDiffuseColor(colorDiffuse);

	static float3 colorSpecular = GetSpecularColor();
	ImGui::Text("Specular Color:"); ImGui::SameLine();
	if (ImGui::ColorEdit3("##Specular Color", (float*)&colorSpecular))
		SetSpecularColor(colorSpecular);

	ImGui::Text("");

	//remove actual texture
	static UID thisUID = UniqueID::GenerateUID();
	char* removeButtonLabel = new char[30];

	ImGui::Checkbox("Use specular Alpha as shininess", &hasShininessAlpha);
	ImGui::SliderFloat("Shininess", &shininess, 0.1f, 200.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
	ImGui::Separator();

	ImGui::Text("Diffuse Texture");
	if (ImGui::Button("Load a diffuse texture")) {
		ImGui::OpenPopup("Choose a texture");
	}

	ImGui::Separator();

	ImGui::Text("Specular Texture");
	if (ImGui::Button("Load a specular texture")) {
		ImGui::OpenPopup("Choose a texture");
	}

	ImGui::Separator();

	ImGui::Text("Normal Texture");
	if (ImGui::Button("Load a normal texture")) {
		ImGui::OpenPopup("Choose a texture");
	}
	/*texture = textureNormal.lock();
	if (texture)
		hasNormal = true;
	ImGui::Checkbox("Normal slider", &hasNormal);*/
	ImGui::SliderFloat("Normal", &normalStrength, 0.0f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
	ImGui::Separator();
	
	ImGui::Separator();
	ImGui::Text("");

		if (ImGui::BeginTable("##MaterialTable", 2))
		{
			ImGui::EndTable();
		}
	}

	ImGui::Separator();
}

void ComponentMaterial::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["owner"] = (GameObject*)owner;
	meta["removed"] = (bool)canBeRemoved;

	std::shared_ptr<ResourceMaterial> materialAsShared = material.lock();
	UID uidMaterial = 0;

	if (materialAsShared)
	{
		uidMaterial = materialAsShared->GetUID();
	}
	meta["materialUID"] = (UID)uidMaterial;

	SaveUIDOfResourceToMeta(meta, "textureDiffuseUID", textureDiffuse.lock());
	SaveUIDOfResourceToMeta(meta, "textureNormalUID", textureNormal.lock());
	SaveUIDOfResourceToMeta(meta, "textureOcclusionUID", textureOcclusion.lock());
	SaveUIDOfResourceToMeta(meta, "textureSpecularUID", textureSpecular.lock());

	meta["diffuseColor_X"] = (float)diffuseColor.x;
	meta["diffuseColor_Y"] = (float)diffuseColor.y;
	meta["diffuseColor_Z"] = (float)diffuseColor.z;

	meta["specularColor_X"] = (float)specularColor.x;
	meta["specularColor_Y"] = (float)specularColor.y;
	meta["specularColor_Z"] = (float)specularColor.z;

	meta["shininess"] = (float)shininess;
	meta["normalStrenght"] = (float)normalStrength;

	meta["hasShininessAlpha"] = (bool)hasShininessAlpha;
}

void ComponentMaterial::SaveUIDOfResourceToMeta(Json& meta, const char* field, const std::weak_ptr<ResourceTexture>& texturePtr)
{
	std::shared_ptr<ResourceTexture> textureAsShared = texturePtr.lock();
	UID uidTexture = 0;

	if (textureAsShared)
	{
		uidTexture = textureAsShared->GetUID();
	}
	meta[field] = (UID)uidTexture;

}

void ComponentMaterial::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	//owner = (GameObject*) meta["owner"];
	canBeRemoved = (bool)meta["removed"];

	UID uidMaterial = meta["materialUID"];

	SetMaterial(App->resources->RequestResource<ResourceMaterial>(uidMaterial).lock());

	diffuseColor.x = (float)meta["diffuseColor_X"];
	diffuseColor.y = (float)meta["diffuseColor_Y"];
	diffuseColor.z = (float)meta["diffuseColor_Z"];

	specularColor.x = (float)meta["specularColor_X"];
	specularColor.y = (float)meta["specularColor_Y"];
	specularColor.z = (float)meta["specularColor_Z"];

	shininess = (float)meta["shininess"];
	normalStrength = (float)meta["normalStrenght"];

	hasShininessAlpha = (bool)meta["hasShininessAlpha"];
}

void ComponentMaterial::SetDiffuseUID(UID& diffuseUID)
{
	this->diffuseUID = diffuseUID;
	LoadTexture(TextureType::DIFFUSE);
}

void ComponentMaterial::SetNormalUID(UID& normalUID)
{
	this->normalUID = normalUID;
	LoadTexture(TextureType::NORMAL);
}

void ComponentMaterial::SetOcclusionUID(UID& occlusionUID)
{
	this->occlusionUID = occlusionUID;
	LoadTexture(TextureType::OCCLUSION);
}

void ComponentMaterial::SetSpecularUID(UID& specularUID)
{
	this->specularUID = specularUID;
	LoadTexture(TextureType::SPECULAR);
}

void ComponentMaterial::LoadTexture()
{
	//TODO User can change the Texture UID on the JSON
	//This destroys the changes of the user

	std::shared_ptr<ResourceTexture> texture;
	//Load Diffuse
	textureDiffuse = App->resources->RequestResource<ResourceTexture>(diffuseUID).lock();
	texture = textureDiffuse.lock();
	if (texture)
	{
		texture->Load();
	}
	//Load Normal
	textureNormal = App->resources->RequestResource<ResourceTexture>(normalUID).lock();
	texture = textureNormal.lock();
	if (texture)
	{
		texture->Load();
	}
	//Load Occlusion
	textureOcclusion = App->resources->RequestResource<ResourceTexture>(occlusionUID).lock();
	texture = textureOcclusion.lock();
	if (texture)
	{
		texture->Load();
	}
	//Load Specular
	textureSpecular = App->resources->RequestResource<ResourceTexture>(specularUID).lock();
	texture = textureSpecular.lock();
	if (texture)
	{
		texture->Load();
	}
}

void ComponentMaterial::LoadTexture(TextureType textureType)
{
	//TODO User can change the Texture UID on the JSON
	//This destroys the changes of the user

	std::shared_ptr<ResourceTexture> texture;
	switch (textureType)
	{
	case TextureType::DIFFUSE:
		textureDiffuse = App->resources->RequestResource<ResourceTexture>(diffuseUID).lock();
		texture = textureDiffuse.lock();
		if (texture)
		{
			texture->Load();
		}
		break;
	case TextureType::NORMAL:
		textureNormal = App->resources->RequestResource<ResourceTexture>(normalUID).lock();
		texture = textureNormal.lock();
		if (texture)
		{
			texture->Load();
		}
		break;
	case TextureType::OCCLUSION:
		textureOcclusion = App->resources->RequestResource<ResourceTexture>(occlusionUID).lock();
		texture = textureOcclusion.lock();
		if (texture)
		{
			texture->Load();
		}
		break;
	case TextureType::SPECULAR:
		textureSpecular = App->resources->RequestResource<ResourceTexture>(specularUID).lock();
		texture = textureSpecular.lock();
		if (texture)
		{
			texture->Load();
		}
		break;
	}
}

void ComponentMaterial::SetMaterial(const std::weak_ptr<ResourceMaterial>& newMaterial)
{
	material = newMaterial;
	std::shared_ptr<ResourceMaterial> materialAsShared = material.lock();

	if (materialAsShared)
	{
		materialAsShared->Load();
		diffuseUID = materialAsShared->GetDiffuseUID();
		normalUID = materialAsShared->GetNormalUID();
		occlusionUID = materialAsShared->GetOcclusionrUID();
		specularUID = materialAsShared->GetSpecularUID();
		LoadTexture();
	}
}