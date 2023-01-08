#include "MeshImporter.h"
#include "FileSystem/Data.h"
#include "EngineLog.h"

/*void MeshImporter::Import(const aiMesh* mesh, DataMesh* ourMesh)
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
}*/

uint64_t MeshImporter::Save(const std::shared_ptr<ResourceMesh> ourMesh, char* &fileBuffer)
{
	
	unsigned int header[2] = { ourMesh->GetNumIndexes(), ourMesh->GetNumVertices()};
	
	unsigned int size = sizeof(header) + ourMesh->GetNumFaces() * (sizeof(unsigned int) * ourMesh->GetNumIndexes()) 
		+ sizeof(float3) * ourMesh->GetNumVertices() + sizeof(float3) * ourMesh->GetNumVertices();
	
	char* cursor = new char[size];
	
	fileBuffer = cursor;
	
	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	bytes = ourMesh->GetNumFaces() * (sizeof(unsigned int) * ourMesh->GetNumIndexes());
	memcpy(cursor, &ourMesh->GetFacesIndices(), bytes);

	cursor += bytes;

	bytes = sizeof(float3) * ourMesh->GetNumVertices();
	memcpy(cursor, &ourMesh->GetVertices(), bytes);

	cursor += bytes;

	bytes = sizeof(float3) * ourMesh->GetNumVertices();
	memcpy(cursor, &ourMesh->GetTextureCoords(), bytes);


	// Provisional return, here we have to return serialize UID for the object
	return 0;
}

void MeshImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceMesh> ourMesh)
{
	unsigned int header[2];
	memcpy(header, fileBuffer, sizeof(header));

	ourMesh->SetNumIndexes(header[0]);
	ourMesh->SetNumVertices(header[1]);

	fileBuffer += sizeof(header);
	
	int size = ourMesh->GetNumIndexes() * ourMesh->GetNumFaces();
	unsigned int* facesPointer = new unsigned int[size];
	memcpy(facesPointer, fileBuffer, ourMesh->GetNumFaces() * (sizeof(unsigned int) * ourMesh->GetNumIndexes()));
	std::vector<std::vector<unsigned int> > faces(facesPointer, facesPointer + ourMesh->GetNumFaces());
	ourMesh->SetFacesIndices(faces);

	float3* vertexPointer = new float3[ourMesh->GetNumVertices()];
	memcpy(vertexPointer, fileBuffer, sizeof(float3) * ourMesh->GetNumVertices());
	std::vector<float3> vertexs(vertexPointer, vertexPointer + ourMesh->GetNumVertices());
	ourMesh->SetVertices(vertexs);

	float3* textureCoordPointer = new float3[ourMesh->GetNumVertices()];
	memcpy(textureCoordPointer, fileBuffer, sizeof(float3) * ourMesh->GetNumVertices());
	std::vector<float3> textureCoord(textureCoordPointer, textureCoordPointer + ourMesh->GetNumVertices());
	ourMesh->SetVertices(textureCoord);
}