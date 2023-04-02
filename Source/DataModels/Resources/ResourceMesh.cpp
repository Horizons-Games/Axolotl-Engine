#pragma warning (disable: 26495)

#include "ResourceMesh.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"

#include "GL/glew.h"
#include "Math/float2.h"
#include "Math/float4x4.h"
#include "Geometry/Triangle.h"

ResourceMesh::ResourceMesh(UID resourceUID, const std::string& fileName, const std::string& assetsPath,
	const std::string& libraryPath) : Resource(resourceUID, fileName, assetsPath, libraryPath),
	vbo(0), ebo(0), vao(0), numVertices(0), numFaces(0), numIndexes(0), numBones(0), materialIndex(0)
{
}

ResourceMesh::~ResourceMesh()
{
	Unload();

	bones.clear();
	attaches.clear();
}

void ResourceMesh::InternalLoad()
{
	CreateVBO();
	CreateEBO();
	CreateVAO();
}

void ResourceMesh::InternalUnload()
{
	bones.clear();

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

	for (unsigned int i = 0; i < numVertices; ++i)
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

	for (unsigned int i = 0; i < numFaces; ++i)
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
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * (3 + 2) * numVertices));

	//tangents
	if (tangents.size() != 0)
	{
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * (3 + 2 + 3) * numVertices));
	}
}

// For mouse-picking purposes
const std::vector<Triangle> ResourceMesh::RetrieveTriangles(const float4x4& modelMatrix)
{
	if (!IsLoaded())
	{
		Load();
	}

	// Vertices
	std::vector<float3> vertices;
	for (unsigned i = 0; i < numVertices; ++i) 
	{
		// Adapt the mesh vertices to the model matrix of its gameobject transform
		vertices.push_back((modelMatrix.MulPos(this->vertices[i])));
	}

	// Triangles
	std::vector<Triangle> triangles;
	triangles.reserve(numFaces);

	for (unsigned i = 0; i < numFaces; ++i)
	{
		// Retrieve the triangles from the vertices adapted to the model matrix
		triangles.push_back(Triangle(
			vertices[facesIndices[i][0]], vertices[facesIndices[i][1]], vertices[facesIndices[i][2]]));
	}

	return triangles;
}