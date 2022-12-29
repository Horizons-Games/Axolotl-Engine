#pragma once
#include "Math/float3.h"

struct DataMesh {
	unsigned int numIndices;
	unsigned int numVertices;
	unsigned int* indices;
	float3* vertices;
};