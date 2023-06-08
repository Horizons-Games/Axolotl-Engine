#pragma once
#include "Resource.h"
//#include "Recast/SampleInterfaces.h"
//#include "Utils/Buffer.h"
#include <vector>


class ResourceNavMesh : virtual public Resource
{
public:
	ResourceNavMesh();
	virtual ~ResourceNavMesh() override;

	//bool Build(Scene* scene);
	//void DrawGizmos(Scene* scene);	
	//void Load(Buffer<char>& buffer);
	//void CleanUp();
	//Buffer<char> Save();

	//bool IsGenerated() const;	
	//dtCrowd* GetCrowd() const;
	//dtNavMeshQuery* GetNavMeshQuery() const;
	//dtNavMesh* GetNavMesh() const;
	//dtTileCache* GetTileCache() const;

public:
	enum DrawMode
	{
		DRAWMODE_NAVMESH,
		DRAWMODE_NAVMESH_TRANS,
		DRAWMODE_NAVMESH_BVTREE,
		DRAWMODE_NAVMESH_NODES,
		DRAWMODE_NAVMESH_PORTALS,
		DRAWMODE_NAVMESH_INVIS,
		DRAWMODE_MESH,
		DRAWMODE_CACHE_BOUNDS,
		MAX_DRAWMODE
	};

	float agentHeight = 2.0f;
	float agentRadius = 0.5f;
	float agentMaxClimb = 0.9f;
	float agentMaxSlope = 45.0f;

	float cellSize = 0.30f;
	float cellHeight = 0.20f;

	int regionMinSize = 8;
	int regionMergeSize = 20;

	int partitionType = 0;

	int edgeMaxLen = 12;
	float edgeMaxError = 1.3f;
	int vertsPerPoly = 6;

	int detailSampleDist = 6;
	int detailSampleMaxError = 1;

	bool keepInterResults = false;

	int tileSize = 48;
	int maxTiles = 0;
	int maxPolysPerTile = 0;

	DrawMode drawMode = DRAWMODE_NAVMESH;

//private:
	//void InitCrowd();

//private:
	//BuildContext* ctx = nullptr;

	//class dtNavMesh* navMesh = nullptr;
	//class dtNavMeshQuery* navQuery = nullptr;
	//class dtCrowd* crowd = nullptr;

	//class dtTileCache* tileCache = nullptr;

	//struct LinearAllocator* talloc = nullptr;
	//struct FastLZCompressor* tcomp = nullptr;
	//struct MeshProcess* tmproc = nullptr;

	//unsigned char navMeshDrawFlags = 0;
};
