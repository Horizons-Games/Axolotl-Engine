#pragma once
#include "Math/float3.h"
#include "FileSystem/UniqueID.h"
#include <vector>

struct DataMesh 
{
	unsigned int numIndices;
	unsigned int numVertices;
	unsigned int* indices;
	float3* vertices;
};

struct DataTexture
{

	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int format = 0;
	unsigned int internalFormat = 0;
	unsigned int type = 0;
	unsigned int pixelsSize;
	unsigned char* pixels = nullptr;
};

struct DataModel
{
	std::vector<UID> texturesUIDs;
	std::vector<UID> meshesUIDs;
};