#include "MeshImporter.h"
#include "FileSystem/Data.h"
#include "EngineLog.h"

void MeshImporter::Import(const aiMesh* mesh, DataMesh* ourMesh)
{
	ourMesh->numIndices = mesh->mNumFaces * 3;
	ourMesh->numVertices = mesh->mNumVertices;

	ourMesh->vertices = new float3[ourMesh->numVertices];

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) 
	{
		ourMesh->vertices[i] = float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
	}

	ourMesh->indices = new unsigned int[ourMesh->numIndices];

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) 
	{
		ourMesh->indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
		ourMesh->indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
		ourMesh->indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
	}
}
uint64_t MeshImporter::Save(const DataMesh* ourMesh, char* &fileBuffer)
{
	
	unsigned int header[2] = { ourMesh->numIndices, ourMesh->numVertices };
	
	unsigned int size = sizeof(header) + sizeof(unsigned int) * ourMesh->numIndices + sizeof(float3) * ourMesh->numVertices;
	
	char* cursor = new char[size];
	
	fileBuffer = cursor;
	
	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	bytes = sizeof(unsigned int) * ourMesh->numIndices;
	memcpy(cursor, ourMesh->indices, bytes);

	cursor += bytes;

	bytes = sizeof(float3) * ourMesh->numVertices;
	memcpy(cursor, ourMesh->vertices, bytes);


	// Provisional return, here we have to return serialize UID for the object
	return 0;
}

void MeshImporter::Load(const char* fileBuffer, DataMesh* ourMesh)
{
	unsigned int header[2];
	memcpy(header, fileBuffer, sizeof(header));

	ourMesh->numIndices = header[0];
	ourMesh->numVertices = header[1];

	fileBuffer += sizeof(header);

	ourMesh->indices = new unsigned int[ourMesh->numIndices];

	
	memcpy(ourMesh->indices, fileBuffer, sizeof(unsigned int) * ourMesh->numIndices);

	fileBuffer += sizeof(unsigned int) * ourMesh->numIndices;


	ourMesh->vertices = new float3[ourMesh->numVertices];
	memcpy(ourMesh->vertices, fileBuffer, sizeof(float3) * ourMesh->numVertices);
}