#include "StdAfx.h"

#include "NavMeshImporter.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "Resources/ResourceNavMesh.h"
#include "DetourTileCache/DetourTileCache.h"

#include "Defines/ExtensionDefines.h"

void NavMeshImporter::Import(const char* filePath, std::shared_ptr<ResourceNavMesh> resource)
{
	char* loadBuffer;
	App->GetModule<ModuleFileSystem>()->Load(filePath, loadBuffer);
	Load(loadBuffer, resource);

	char* saveBuffer{};
	unsigned int size;
	Save(resource, saveBuffer, size);
	App->GetModule<ModuleFileSystem>()->Save(
		(resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), saveBuffer, size);

	delete loadBuffer;
	delete saveBuffer;
}

void NavMeshImporter::Save(const std::shared_ptr<ResourceNavMesh>& resource, char*& fileBuffer, unsigned int& size)
{
	// Mesh and Tile parameters + header
	size = (sizeof(float) * 7 + sizeof(int) * 10 + sizeof(bool) + sizeof(unsigned char)) 
		   + sizeof(NavMeshHeader);

	NavMeshHeader header = resource->GetNavMeshHeader();

	dtTileCache* tileCache = resource->GetTileCache();
	if (tileCache != nullptr)
	{
		for (int i = 0; i < tileCache->getTileCount(); ++i)
		{
			const dtCompressedTile* tile = tileCache->getTile(i);
			if (!tile || !tile->header || !tile->dataSize)
			{
				continue;
			}

			size += sizeof(int) * 2;						// TileHeader
			size += sizeof(unsigned char) * tile->dataSize; // Tile content
			header.numTiles++;
		}
	}

	char* cursor = new char[size]{};

	fileBuffer = cursor;

	unsigned int bytes = sizeof(NavMeshHeader);
	memcpy(cursor, &header, bytes);
	cursor += bytes;

	bytes = sizeof(float);

	float agentHeight = resource->GetAgentHeight();
	memcpy(cursor, &agentHeight, bytes);
	cursor += bytes;

	float agentRadius = resource->GetAgentRadius();
	memcpy(cursor, &agentRadius, bytes);
	cursor += bytes;

	float agentMaxClimb = resource->GetAgentMaxClimb();
	memcpy(cursor, &agentMaxClimb, bytes);
	cursor += bytes;

	float agentMaxSlope = resource->GetAgentMaxSlope();
	memcpy(cursor, &agentMaxSlope, bytes);
	cursor += bytes;

	float cellSize = resource->GetCellSize();
	memcpy(cursor, &cellSize, bytes);
	cursor += bytes;

	float cellHeight = resource->GetCellHeight();
	memcpy(cursor, &cellHeight, bytes);
	cursor += bytes;

	float edgeMaxError = resource->GetEdgeMaxError();
	memcpy(cursor, &edgeMaxError, bytes);
	cursor += bytes;

	bytes = sizeof(int);

	int regionMinSize = resource->GetRegionMinSize();
	memcpy(cursor, &regionMinSize, bytes);
	cursor += bytes;

	int regionMergeSize = resource->GetRegionMergeSize();
	memcpy(cursor, &regionMergeSize, bytes);
	cursor += bytes;

	int partitionType = resource->GetPartitionType();
	memcpy(cursor, &partitionType, bytes);
	cursor += bytes;

	int edgeMaxLen = resource->GetEdgeMaxLen();
	memcpy(cursor, &edgeMaxLen, bytes);
	cursor += bytes;

	int vertsPerPoly = resource->GetVertsPerPoly();
	memcpy(cursor, &vertsPerPoly, bytes);
	cursor += bytes;

	int detailSampleDist = resource->GetDetailSampleDist();
	memcpy(cursor, &detailSampleDist, bytes);
	cursor += bytes;

	int detailSampleMaxError = resource->GetDetailSampleMaxError();
	memcpy(cursor, &detailSampleMaxError, bytes);
	cursor += bytes;

	int tileSize = resource->GetTileSize();
	memcpy(cursor, &tileSize, bytes);
	cursor += bytes;

	int maxTiles = resource->GetMaxTiles();
	memcpy(cursor, &maxTiles, bytes);
	cursor += bytes;

	int maxPolysPerTile = resource->GetMaxPolysPerTile();
	memcpy(cursor, &maxPolysPerTile, bytes);
	cursor += bytes;

	bytes = sizeof(bool);

	bool keepInterResults = resource->GetKeepInterResults();
	memcpy(cursor, &keepInterResults, bytes);
	cursor += bytes;

	bytes = sizeof(unsigned char);

	unsigned char navMeshDrawFlags = resource->GetNavMeshDrawFlags();
	memcpy(cursor, &navMeshDrawFlags, bytes);
	cursor += bytes;

	if (tileCache == nullptr)
	{
		return;
	}

	for (int i = 0; i < tileCache->getTileCount(); i++)
	{
		const dtCompressedTile* tile = tileCache->getTile(i);
		if (!tile || !tile->header || !tile->dataSize)
		{
			continue;
		}

		unsigned int tileHeader[2] = 
		{ 
			tileCache->getTileRef(tile), 
			tile->dataSize
		};

		bytes = sizeof(tileHeader);

		memcpy(cursor, tileHeader, bytes);
		cursor += bytes;

		bytes = sizeof(unsigned char) * tile->dataSize;
		memcpy_s(cursor, bytes, tile->data, bytes);
		cursor += bytes;
	}
}

void NavMeshImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceNavMesh> resource)
{
	NavMeshHeader header;

	unsigned int bytes = sizeof(NavMeshHeader);
	memcpy(&header, fileBuffer, bytes);
	fileBuffer += bytes;

	bytes = sizeof(float);

	float* agentHeight = new float;
	memcpy(agentHeight, fileBuffer, bytes);
	resource->SetAgentHeight(*agentHeight);

	delete agentHeight;

	fileBuffer += bytes;

	float* agentRadius = new float;
	memcpy(agentRadius, fileBuffer, bytes);
	resource->SetAgentRadius(*agentRadius);

	delete agentRadius;

	fileBuffer += bytes;

	float* agentMaxClimb = new float;
	memcpy(agentMaxClimb, fileBuffer, bytes);
	resource->SetAgentMaxClimb(*agentMaxClimb);

	delete agentMaxClimb;

	fileBuffer += bytes;

	float* agentMaxSlope = new float;
	memcpy(agentMaxSlope, fileBuffer, bytes);
	resource->SetAgentMaxSlope(*agentMaxSlope);

	delete agentMaxSlope;

	fileBuffer += bytes;

	float* cellSize = new float;
	memcpy(cellSize, fileBuffer, bytes);
	resource->SetCellSize(*cellSize);

	delete cellSize;

	fileBuffer += bytes;

	float* cellHeight = new float;
	memcpy(cellHeight, fileBuffer, bytes);
	resource->SetCellHeight(*cellHeight);

	delete cellHeight;

	fileBuffer += bytes;

	float* edgeMaxError = new float;
	memcpy(edgeMaxError, fileBuffer, bytes);
	resource->SetEdgeMaxError(*edgeMaxError);

	delete edgeMaxError;

	fileBuffer += bytes;

	bytes = sizeof(int);

	int* regionMinSize = new int;
	memcpy(regionMinSize, fileBuffer, bytes);
	resource->SetRegionMinSize(*regionMinSize);

	delete regionMinSize;

	fileBuffer += bytes;

	int* regionMergeSize = new int;
	memcpy(regionMergeSize, fileBuffer, bytes);
	resource->SetRegionMergeSize(*regionMergeSize);

	delete regionMergeSize;

	fileBuffer += bytes;

	int* partitionType = new int;
	memcpy(partitionType, fileBuffer, bytes);
	resource->SetPartitionType(*partitionType);

	delete partitionType;

	fileBuffer += bytes;

	int* edgeMaxLen = new int;
	memcpy(edgeMaxLen, fileBuffer, bytes);
	resource->SetEdgeMaxLen(*edgeMaxLen);

	delete edgeMaxLen;

	fileBuffer += bytes;

	int* vertsPerPoly = new int;
	memcpy(vertsPerPoly, fileBuffer, bytes);
	resource->SetVertsPerPoly(*vertsPerPoly);

	delete vertsPerPoly;

	fileBuffer += bytes;

	int* detailSampleDist = new int;
	memcpy(detailSampleDist, fileBuffer, bytes);
	resource->SetDetailSampleDist(*detailSampleDist);

	delete detailSampleDist;

	fileBuffer += bytes;

	int* detailSampleMaxError = new int;
	memcpy(detailSampleMaxError, fileBuffer, bytes);
	resource->SetDetailSampleMaxError(*detailSampleMaxError);

	delete detailSampleMaxError;

	fileBuffer += bytes;

	int* tileSize = new int;
	memcpy(tileSize, fileBuffer, bytes);
	resource->SetTileSize(*tileSize);

	delete tileSize;

	fileBuffer += bytes;

	int* maxTiles = new int;
	memcpy(maxTiles, fileBuffer, bytes);
	resource->SetMaxTiles(*maxTiles);

	delete maxTiles;

	fileBuffer += bytes;

	int* maxPolysPerTile = new int;
	memcpy(maxPolysPerTile, fileBuffer, bytes);
	resource->SetMaxPolysPerTile(*maxPolysPerTile);

	delete maxPolysPerTile;

	fileBuffer += bytes;

	bytes = sizeof(bool);

	bool* keepInterResults = new bool;
	memcpy(keepInterResults, fileBuffer, bytes);
	resource->SetKeepInterResults(*keepInterResults);

	delete keepInterResults;

	fileBuffer += bytes;

	bytes = sizeof(unsigned char);

	unsigned char* navMeshDrawFlags = new unsigned char;
	memcpy(navMeshDrawFlags, fileBuffer, bytes);
	resource->SetNavMeshDrawFlags(*navMeshDrawFlags);

	delete navMeshDrawFlags;

	fileBuffer += bytes;
	if (header.numTiles == 0 || header.maxTiles == 0)
	{
		return;
	}

	resource->InitNavMesh(header.bmin);
	resource->InitTileCache(header.bmin, header.maxTiles);
	for (int i = 0; i < header.numTiles; i++)
	{
		unsigned int tileHeader[2];

		bytes = sizeof(tileHeader);
		memcpy(tileHeader, fileBuffer, bytes);
		fileBuffer += bytes;

		if (!tileHeader[0] || !tileHeader[1])
		{
			break;
		}

		unsigned char* data = (unsigned char*) malloc(tileHeader[1]);
		if (!data)
		{
			break;
		}
		memset(data, 0, tileHeader[1]);
		bytes = sizeof(unsigned char) * tileHeader[1];
		memcpy_s(data, bytes, fileBuffer, bytes);
		fileBuffer += bytes;
		resource->AddTile(data, tileHeader[1]);
	}
	resource->InitCrowd();
}