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

							//position			//uv				//normal		
	unsigned vertexSize = (sizeof(float) * 3 + sizeof(float) * 2 + sizeof(float) * 3);
	//tangents
	if (tangents.size() != 0)
	{
		vertexSize += sizeof(float) * 3;
	}
	//unsigned vertexSize = (sizeof(float) * 3 + sizeof(float) * 2);
	GLuint bufferSize = vertexSize * numVertices;

	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

	GLuint positionSize = sizeof(float) * 3 * numVertices;

	glBufferSubData(GL_ARRAY_BUFFER, 0, positionSize, &(vertices[0]));

	GLuint uvOffset = positionSize;
	GLuint uvSize = sizeof(float) * 2 * numVertices;

	float2* uvs = (float2*)(glMapBufferRange(GL_ARRAY_BUFFER, uvOffset, uvSize, GL_MAP_WRITE_BIT));

	for (int i = 0; i < numVertices; ++i)
	{
		uvs[i] = float2(textureCoords[i].x, textureCoords[i].y);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	unsigned normalsOffset = positionSize + uvSize;
	unsigned normalsSize = sizeof(float) * 3 * numVertices;
	glBufferSubData(GL_ARRAY_BUFFER, normalsOffset, normalsSize, &normals[0]);

	if (tangents.size() != 0)
	{
		unsigned tangentsOffset = positionSize + uvSize + normalsSize;
		unsigned tangentsSize = sizeof(float) * 3 * numVertices;
		glBufferSubData(GL_ARRAY_BUFFER, tangentsOffset, tangentsSize, &tangents[0]);
	}

}

void ResourceMesh::CreateEBO()
{
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	GLuint indexSize = sizeof(GLuint) * numFaces * 3;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, nullptr, GL_STATIC_DRAW);

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

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//texCoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 3 * numVertices));

	//normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * (3 + 2) * this->numVertices));

	//tangents
	if (tangents.size() != 0)
	{
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * (3 + 2 + 3) * this->numVertices));
	}
}
