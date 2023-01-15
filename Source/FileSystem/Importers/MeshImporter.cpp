#include "MeshImporter.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"

#include "EngineLog.h"

void MeshImporter::Import(const char* filePath, std::shared_ptr<ResourceMesh> resource)
{
	char* loadBuffer{};
	App->fileSystem->Load(filePath, loadBuffer);
	Load(loadBuffer, resource);

	char* saveBuffer{};
	unsigned int size;
	Save(resource, saveBuffer, size);
	App->fileSystem->Save((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), saveBuffer, size);
}

uint64_t MeshImporter::Save(const std::shared_ptr<ResourceMesh>& resource, char* &fileBuffer, unsigned int& size)
{
	unsigned int header[3] = { resource->GetNumFaces(), resource->GetNumVertices(), resource->GetMaterialIndex()};
	
	size = sizeof(header) + resource->GetNumFaces() * (sizeof(unsigned int) * 3) 
		+ sizeof(float3) * resource->GetNumVertices() * 4;
	
	char* cursor = new char[size];
	
	fileBuffer = cursor;
	
	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	bytes = sizeof(float3) * resource->GetNumVertices();
	memcpy(cursor, &resource->GetVertices(), bytes);

	cursor += bytes;

	bytes = sizeof(float3) * resource->GetNumVertices();
	memcpy(cursor, &resource->GetTextureCoords(), bytes);

	cursor += bytes;

	bytes = sizeof(float3) * resource->GetNumVertices();
	memcpy(cursor, &resource->GetNormals(), bytes);

	cursor += bytes;

	bytes = sizeof(float3) * resource->GetNumVertices();
	memcpy(cursor, &resource->GetTangents(), bytes);

	cursor += bytes;

	for (int i = 0; i < resource->GetNumFaces(); ++i)
	{
		bytes = sizeof(unsigned int) * 3;
		memcpy(cursor, &(resource->GetFacesIndices()[i][0]), bytes);

		cursor += bytes;
	}

	// Provisional return, here we have to return serialize UID for the object
	return 0;
}

void MeshImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceMesh> resource)
{
	unsigned int header[4];
	memcpy(header, fileBuffer, sizeof(header));

	resource->SetNumFaces(header[0]);
	resource->SetNumVertices(header[1]);
	resource->SetMaterialIndex(header[2]);
	bool hasTangents = header[3];

	fileBuffer += sizeof(header);

	float3* vertexPointer = new float3[resource->GetNumVertices()];
	unsigned int bytes = sizeof(float3) * resource->GetNumVertices();
	memcpy(vertexPointer, fileBuffer, bytes);
	std::vector<float3> vertexs(vertexPointer, vertexPointer + resource->GetNumVertices());
	resource->SetVertices(vertexs);

	fileBuffer += bytes;

	float3* textureCoordPointer = new float3[resource->GetNumVertices()];
	bytes = sizeof(float3) * resource->GetNumVertices();
	memcpy(textureCoordPointer, fileBuffer, bytes);
	std::vector<float3> textureCoord(textureCoordPointer, textureCoordPointer + resource->GetNumVertices());
	resource->SetTextureCoords(textureCoord);

	fileBuffer += bytes;

	float3* normalPointer = new float3[resource->GetNumVertices()];
	bytes = sizeof(float3) * resource->GetNumVertices();
	memcpy(normalPointer, fileBuffer, bytes);
	std::vector<float3> normals(normalPointer, normalPointer + resource->GetNumVertices());
	resource->SetNormals(normals);

	fileBuffer += bytes;

	if(hasTangents)
	{
		float3* tangentPointer = new float3[resource->GetNumVertices()];
		bytes = sizeof(float3) * resource->GetNumVertices();
		memcpy(tangentPointer, fileBuffer, bytes);
		std::vector<float3> tangents(tangentPointer, tangentPointer + resource->GetNumVertices());
		resource->SetTangents(tangents);

		fileBuffer += bytes;
	}

	int size = 3 * resource->GetNumFaces();
	unsigned int* indexesPointer = new unsigned int[size];
	bytes = resource->GetNumFaces() * sizeof(unsigned int) * 3;
	memcpy(indexesPointer, fileBuffer, bytes);
	std::vector<unsigned int> aux(indexesPointer, indexesPointer + resource->GetNumFaces() * 3);
	std::vector<std::vector<unsigned int>> faces;
	for (int i = 0; i + 2 < (resource->GetNumFaces() * 3); i += 3) 
	{
		std::vector<unsigned int> indexes{ aux[i], aux[i + 1], aux[i + 2] };
		faces.push_back(indexes);
	}
	resource->SetFacesIndices(faces);

}