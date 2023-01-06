#include "Mesh.h"

#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEngineCamera.h"
#include "ResourceMaterial.h"

#include <GL/glew.h>

Mesh::Mesh(const aiMesh* mesh)
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

Mesh::~Mesh()
{
	ENGINE_LOG("Deleting Mesh");

	glDeleteBuffers(1, &this->vbo);
	glDeleteBuffers(1, &this->ebo);
	glDeleteVertexArrays(1, &this->vao);

	delete vertices;
}

void Mesh::LoadVBO(const aiMesh* mesh)
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	unsigned vertexSize = (sizeof(float) * 3 + sizeof(float) * 2);
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

	ENGINE_LOG("Generated VAO %i", vao);
}

void Mesh::Draw(const std::vector<unsigned>& modelTextures,
	const float3 &translation, const float4x4&rotation, const float3 &scale)
{
	unsigned program = App->program->GetProgram();
	const float4x4& view = App->engineCamera->GetViewMatrix();
	const float4x4& proj = App->engineCamera->GetProjectionMatrix();
	const float4x4& model = float4x4::FromTRS(translation, rotation, scale);

	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

	ResourceMaterial mat;
	mat.bind(program);

	const float3 position (0.0f, 4.0f, 0.0f);
	const float3 ambient(0.2f, 0.2f, 0.2f);
	const float3 diffuse(0.5f, 0.5f, 0.5f);
	const float3 specular(0.0f, 4.0f, 0.0f);

	glUniform3f(glGetUniformLocation(program, "light.position"), position.x, position.y, position.z);
	glUniform3f(glGetUniformLocation(program, "light.ambient"), ambient.x, ambient.y, ambient.z);
	glUniform3f(glGetUniformLocation(program, "light.diffuse"), diffuse.x, diffuse.y, diffuse.z);
	glUniform3f(glGetUniformLocation(program, "light.specular"), specular.x, specular.y, specular.z);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, modelTextures[this->materialIndex]);
	glUniform1i(glGetUniformLocation(program, "diffuse"), 0);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glDrawElements(GL_TRIANGLES, this->numIndexes, GL_UNSIGNED_INT, nullptr);
}
