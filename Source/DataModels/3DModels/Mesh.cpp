#include "Mesh.h"

#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEngineCamera.h"
#include "DataModels/Resources/ResourceMesh.h"

#include <GL/glew.h>
#include <DataModels/3DModels/Model.h>

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	ENGINE_LOG("Deleting Mesh");

	glDeleteBuffers(1, &this->vbo);
	glDeleteBuffers(1, &this->ebo);

	glDeleteVertexArrays(1, &this->vao);

	delete vertices;
}

void Mesh::Load(const aiMesh* mesh)
{
	ENGINE_LOG("--- Loading mesh ---");

	LoadVBO(mesh);
	LoadEBO(mesh);
	CreateVAO();

	//ResourceMaterial resmat;
	//resmat.bind(program);

	this->materialIndex = mesh->mMaterialIndex;
	this->vertices = new vec[mesh->mNumVertices];

	for (int i = 0; i < mesh->mNumVertices; ++i)
	{
		this->vertices[i] = float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
	}
}

void Mesh::SetFromResource(std::shared_ptr<ResourceMesh>& resource)
{
	materialIndex = resource->GetMaterialIndex();
	numVertices = resource->GetNumVertices();
	numIndexes = resource->GetNumFaces() * 3;

	vbo = resource->GetVBO();
	ebo = resource->GetEBO();
	vao = resource->GetVAO();


	std::vector<float3> vertexPointer = resource->GetVertices();

	this->vertices = new vec[numVertices];

	for (int i = 0; i < numVertices; ++i)
	{
		this->vertices[i] = float3(vertexPointer[i].x, vertexPointer[i].y, vertexPointer[i].z);
	}

}

void Mesh::LoadVBO(const aiMesh* mesh)
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

							//position			//uv				//normal			//tangent
	unsigned vertexSize = (sizeof(float) * 3 + sizeof(float) * 2 + sizeof(float) * 3 + sizeof(float) * 3);
	unsigned bufferSize = vertexSize * mesh->mNumVertices;
	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

	unsigned positionSize = sizeof(float) * 3 * mesh->mNumVertices;
	glBufferSubData(GL_ARRAY_BUFFER, 0, positionSize, mesh->mVertices);

	unsigned uvOffset = positionSize;
	unsigned uvSize = sizeof(float) * 2 * mesh->mNumVertices;
	float2* uvs = (float2*)(glMapBufferRange(GL_ARRAY_BUFFER, uvOffset, uvSize, GL_MAP_WRITE_BIT));

	for (unsigned i = 0; i < mesh->mNumVertices; ++i)
	{
		uvs[i] = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	this->numVertices = mesh->mNumVertices;

	unsigned normalsOffset = positionSize + uvSize;
	unsigned normalsSize = sizeof(float) * 3 * mesh->mNumVertices;
	glBufferSubData(GL_ARRAY_BUFFER, normalsOffset, normalsSize, mesh->mNormals);

	unsigned tangentsOffset = positionSize + uvSize + normalsSize;
	unsigned tangentsSize = sizeof(float) * 3 * mesh->mNumVertices;
	glBufferSubData(GL_ARRAY_BUFFER, tangentsOffset, tangentsSize, mesh->mTangents);

	ENGINE_LOG("Generated VBO %i with %i vertices", vbo, numVertices);
}

void Mesh::LoadEBO(const aiMesh* mesh)
{
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	unsigned indexSize = sizeof(unsigned) * mesh->mNumFaces * 3;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, nullptr, GL_STATIC_DRAW);
	unsigned* indices = (unsigned*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

	for (unsigned i = 0; i < mesh->mNumFaces; ++i)
	{
		assert(mesh->mFaces[i].mNumIndices == 3);
		*(indices++) = mesh->mFaces[i].mIndices[0];
		*(indices++) = mesh->mFaces[i].mIndices[1];
		*(indices++) = mesh->mFaces[i].mIndices[2];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	this->numIndexes = mesh->mNumFaces * 3;

	ENGINE_LOG("Generated EBO %i with %i indexes", ebo, this->numIndexes);
}

void Mesh::CreateVAO()
{
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 3 * this->numVertices));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * (3 + 2) * this->numVertices));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * (3 + 2 + 3) * this->numVertices));

	ENGINE_LOG("Generated VAO %i", vao);
}

void Mesh::Draw(const std::vector<unsigned>& modelTextures, const float3& translation, const float4x4& rotation, const float3& scale)
{
	unsigned program = App->program->GetProgram();
	const float4x4& view = App->engineCamera->GetViewMatrix();
	const float4x4& proj = App->engineCamera->GetProjectionMatrix();
	const float4x4& model = float4x4::FromTRS(translation, rotation, scale);

	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

	const float3 position(0.0f, 4.0f, 0.0f);
	const float3 color(1.f, 1.f, 1.f);

	glUniform3f(glGetUniformLocation(program, "light.position"), position.x, position.y, position.z);
	glUniform3f(glGetUniformLocation(program, "light.color"), color.x, color.y, color.z);
	float3 viewPos = App->engineCamera->GetPosition();
	glUniform3f(glGetUniformLocation(program, "viewPos"), viewPos.x, viewPos.y, viewPos.z);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, modelTextures[this->materialIndex]);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glDrawElements(GL_TRIANGLES, this->numIndexes, GL_UNSIGNED_INT, nullptr);

}

void Mesh::NewDraw(const std::unique_ptr<Material>& material, const float3 &translation, const float4x4&rotation, const float3 &scale)
{
	unsigned program = App->program->GetProgram();
	const float4x4& view = App->engineCamera->GetViewMatrix();
	const float4x4& proj = App->engineCamera->GetProjectionMatrix();
	const float4x4& model = float4x4::FromTRS(translation, rotation, scale);

	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

	//TODO move this to Component (Material?) for the Draw
	//if (material->haveDiffuse) glUniform1i(glGetUniformLocation(program, "material.diffuse_map"), material->diffuse);
	//if (material->haveSpecular) glUniform1i(glGetUniformLocation(program, "material.specular_map"), material->specular);
	glUniform1f(glGetUniformLocation(program, "material.shininess"), material->shininess);

	const float3 position (0.0f, 4.0f, 0.0f);
	const float3 color(1.f, 1.f, 1.f);

	glUniform3f(glGetUniformLocation(program, "light.position"), position.x, position.y, position.z);
	glUniform3f(glGetUniformLocation(program, "light.color"), color.x, color.y, color.z);
	float3 viewPos = App->engineCamera->GetPosition();
	glUniform3f(glGetUniformLocation(program, "viewPos"), viewPos.x, viewPos.y, viewPos.z);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material->diffuse);

	if (material->haveNormal) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material->normal);
		glUniform1i(glGetUniformLocation(program, "material.normal_map"), 1);
	}
	glUniform1i(glGetUniformLocation(program, "material.has_normal_map"), material->haveNormal);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glDrawElements(GL_TRIANGLES, this->numIndexes, GL_UNSIGNED_INT, nullptr);
}

