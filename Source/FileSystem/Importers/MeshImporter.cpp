#include "MeshImporter.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"

#include "EngineLog.h"

void MeshImporter::Import(const char* filePath, std::shared_ptr<ResourceMesh> resource)
{
	char* buffer;

	App->fileSystem->Load(filePath, buffer);
	Load(buffer, resource);
}

uint64_t MeshImporter::Save(const std::shared_ptr<ResourceMesh> resource, char* &fileBuffer)
{
	unsigned int header[2] = { resource->GetNumIndexes(), resource->GetNumVertices()};
	
	unsigned int size = sizeof(header) + resource->GetNumFaces() * (sizeof(unsigned int) * resource->GetNumIndexes()) 
		+ sizeof(float3) * resource->GetNumVertices() + sizeof(float3) * resource->GetNumVertices();
	
	char* cursor = new char[size];
	
	fileBuffer = cursor;
	
	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	bytes = resource->GetNumFaces() * (sizeof(unsigned int) * resource->GetNumIndexes());
	memcpy(cursor, &resource->GetFacesIndices(), bytes);

	cursor += bytes;

	bytes = sizeof(float3) * resource->GetNumVertices();
	memcpy(cursor, &resource->GetVertices(), bytes);

	cursor += bytes;

	bytes = sizeof(float3) * resource->GetNumVertices();
	memcpy(cursor, &resource->GetTextureCoords(), bytes);


	// Provisional return, here we have to return serialize UID for the object
	return 0;
}

void MeshImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceMesh> resource)
{
	unsigned int header[2];
	memcpy(header, fileBuffer, sizeof(header));

	resource->SetNumIndexes(header[0]);
	resource->SetNumVertices(header[1]);

	fileBuffer += sizeof(header);
	
	int size = resource->GetNumIndexes() * resource->GetNumFaces();
	unsigned int* facesPointer = new unsigned int[size];
	memcpy(facesPointer, fileBuffer, resource->GetNumFaces() * (sizeof(unsigned int) * resource->GetNumIndexes()));
	std::vector<std::vector<unsigned int> > faces(facesPointer, facesPointer + resource->GetNumFaces());
	resource->SetFacesIndices(faces);

	float3* vertexPointer = new float3[resource->GetNumVertices()];
	memcpy(vertexPointer, fileBuffer, sizeof(float3) * resource->GetNumVertices());
	std::vector<float3> vertexs(vertexPointer, vertexPointer + resource->GetNumVertices());
	resource->SetVertices(vertexs);

	float3* textureCoordPointer = new float3[resource->GetNumVertices()];
	memcpy(textureCoordPointer, fileBuffer, sizeof(float3) * resource->GetNumVertices());
	std::vector<float3> textureCoord(textureCoordPointer, textureCoordPointer + resource->GetNumVertices());
	resource->SetVertices(textureCoord);
}