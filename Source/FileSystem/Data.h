#pragma once
#include "Math/float3.h"

struct DataMesh 
{
	unsigned int numIndices;
	unsigned int numVertices;
	unsigned int* indices;
	float3* vertices;
};

struct DataTexture
{
	unsigned width;
	unsigned height;
	unsigned format;
	unsigned dataSize;
	unsigned char* imageData = nullptr;
};