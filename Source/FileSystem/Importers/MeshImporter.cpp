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
	unsigned int header[3] = { resource->GetNumFaces(), resource->GetNumVertices(), resource->GetMaterialIndex()};
	
	unsigned int size = sizeof(header) + resource->GetNumFaces() * (sizeof(unsigned int) * 3) 
		+ sizeof(float3) * resource->GetNumVertices() * 2;
	
	char* cursor = new char[size];
	
	fileBuffer = cursor;
	
	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	bytes = resource->GetNumFaces() * (sizeof(unsigned int) * 3);
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

	resource->SetNumFaces(header[0]);
	resource->SetNumVertices(header[1]);

	fileBuffer += sizeof(header);

	int size = 3 * resource->GetNumFaces();
	unsigned int* indexesPointer = new unsigned int[size];
	int bytes = resource->GetNumFaces() * sizeof(unsigned int) * 3;
	memcpy(indexesPointer, fileBuffer, bytes);
	std::vector<std::vector<unsigned int>> faces;
	for (int i = 0; i < resource->GetNumFaces(); ++i) {
		std::vector<unsigned int > indexes(indexesPointer, indexesPointer + 3);
		indexesPointer += (sizeof(unsigned int) * 3);
		faces.push_back(indexes);
	}
	resource->SetFacesIndices(faces);

	fileBuffer += bytes;

	float3* vertexPointer = new float3[resource->GetNumVertices()];
	bytes = sizeof(float3) * resource->GetNumVertices();
	memcpy(vertexPointer, fileBuffer, bytes);
	std::vector<float3> vertexs(vertexPointer, vertexPointer + resource->GetNumVertices());
	resource->SetVertices(vertexs);

	fileBuffer += bytes;

	float3* textureCoordPointer = new float3[resource->GetNumVertices()];
	bytes = sizeof(float3) * resource->GetNumVertices();
	memcpy(textureCoordPointer, fileBuffer, bytes);
	std::vector<float3> textureCoord(textureCoordPointer, textureCoordPointer + resource->GetNumVertices());
	resource->SetTextureCoords(textureCoord);

}