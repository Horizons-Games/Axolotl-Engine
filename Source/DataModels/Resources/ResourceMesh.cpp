#include "ResourceMesh.h"

#include <vector>

#include "GL/glew.h"
#include "Math/float2.h"

void ResourceMesh::Load()
{
	this->CreateVBO();
	this->CreateEBO();
	this->CreateVAO();
}

void ResourceMesh::Unload()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vao);
	vbo = 0;
	ebo = 0;
	vao = 0;
}

void ResourceMesh::CreateVBO()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLuint vertex_size = (sizeof(float) * 3 + sizeof(float) * 2);
	GLuint buffer_size = vertex_size * numVertices;

	glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_STATIC_DRAW);

	GLuint position_size = sizeof(float) * 3 * numVertices;

	glBufferSubData(GL_ARRAY_BUFFER, 0, position_size, &(vertices[0]));

	GLuint uv_offset = position_size;
	GLuint uv_size = sizeof(float) * 2 * numVertices;

	float2* uvs = (float2*)(glMapBufferRange(GL_ARRAY_BUFFER, uv_offset, uv_size, GL_MAP_WRITE_BIT));

	for (int i = 0; i < numVertices; ++i)
	{
		uvs[i] = float2(textureCoords[i].x, textureCoords[i].y);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void ResourceMesh::CreateEBO()
{
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	GLuint index_size = sizeof(GLuint) * numFaces * 3;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, nullptr, GL_STATIC_DRAW);

	GLuint* indices = (GLuint*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

	for (int i = 0; i < numFaces; ++i)
	{
		assert(facesIndices[i].size() == 3); // note: assume triangles = 3 indices per face
		*(indices++) = facesIndices[i][0];
		*(indices++) = facesIndices[i][1];
		*(indices++) = facesIndices[i][2];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void ResourceMesh::CreateVAO()
{
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 3 * numVertices));
}
