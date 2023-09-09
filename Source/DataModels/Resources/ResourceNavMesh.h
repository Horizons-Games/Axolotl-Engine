#pragma once
#include "Resource.h"
#include "Recast/SampleInterfaces.h"
//#include "Utils/Buffer.h"
#include <vector>

class dtNavMesh;
class dtNavMeshQuery;
class dtCrowd;
class dtTileCache;
class Scene;

struct NavMeshHeader
{
	int numTiles;
	int maxTiles;
	float bmin[3];
};

class ResourceNavMesh : virtual public Resource
{
public:
	ResourceNavMesh(UID resourceUID,
					 const std::string& fileName,
					 const std::string& assetsPath,
					 const std::string& libraryPath);
	virtual ~ResourceNavMesh() override;

	ResourceType GetType() const override;

	void SaveImporterOptions(Json& meta) override{};
	void LoadImporterOptions(Json& meta) override{};

	void SaveLoadOptions(Json& meta) override{};
	void LoadLoadOptions(Json& meta) override{};

	bool InitNavMesh(const float* bmin);
	bool InitTileCache(const float* bmin, int maxTiles);
	bool AddTile(unsigned char* data, int dataSize);
	void InitCrowd();

	bool Build(Scene* scene);
	void DrawGizmos(Scene* scene);	
	void CleanUp();

	bool IsGenerated() const;
	dtCrowd* GetCrowd() const;
	dtNavMeshQuery* GetNavMeshQuery() const;
	dtNavMesh* GetNavMesh() const;
	NavMeshHeader GetNavMeshHeader() const;
	dtTileCache* GetTileCache() const;

	float GetAgentHeight() const;
	float GetAgentRadius() const;
	float GetAgentMaxClimb() const;
	float GetAgentMaxSlope() const;
	float GetCellSize() const;
	float GetCellHeight() const;
	float GetEdgeMaxError() const;

	void SetAgentHeight(float agentHeight);
	void SetAgentRadius(float agentRadius);
	void SetAgentMaxClimb(float agentMaxClimb);
	void SetAgentMaxSlope(float agentMaxSlope);
	void SetCellSize(float cellSize);
	void SetCellHeight(float cellHeight);
	void SetEdgeMaxError(float edgeMaxError);

	int GetRegionMinSize() const;
	int GetRegionMergeSize() const;
	int GetPartitionType() const;
	int GetEdgeMaxLen() const;
	int GetVertsPerPoly() const;
	int GetDetailSampleDist() const;
	int GetDetailSampleMaxError() const;
	int GetTileSize() const;
	int GetMaxTiles() const;
	int GetMaxPolysPerTile() const;

	void SetRegionMinSize(int regionMinSize);
	void SetRegionMergeSize(int regionMergeSize);
	void SetPartitionType(int partitionType);
	void SetEdgeMaxLen(int edgeMaxLen);
	void SetVertsPerPoly(int vertsPerPoly);
	void SetDetailSampleDist(int detailSampleDist);
	void SetDetailSampleMaxError(int detailSampleMaxError);
	void SetTileSize(int tileSize);
	void SetMaxTiles(int maxTiles);
	void SetMaxPolysPerTile(int maxPolysPerTile);

	bool GetKeepInterResults() const;

	void SetKeepInterResults(bool keepInterResults);

	unsigned char GetNavMeshDrawFlags() const;

	void SetNavMeshDrawFlags(unsigned char navMeshDrawFlags);

protected:
	void InternalLoad() override{};
	void InternalUnload() override{};

public:
	enum class DrawMode
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

private:

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

	unsigned char navMeshDrawFlags = 0;

	DrawMode drawMode = DrawMode::DRAWMODE_NAVMESH;

	BuildContext* ctx = nullptr;

	dtNavMesh* navMesh = nullptr;
	dtNavMeshQuery* navQuery = nullptr;

	dtCrowd* crowd = nullptr;
	dtTileCache* tileCache = nullptr;

	struct LinearAllocator* talloc = nullptr;
	struct FastLZCompressor* tcomp = nullptr;
	struct MeshProcess* tmproc = nullptr;

};

inline ResourceType ResourceNavMesh::GetType() const
{
	return ResourceType::NavMesh;
}

inline float ResourceNavMesh::GetAgentHeight() const
{
	return agentHeight;
}

inline float ResourceNavMesh::GetAgentRadius() const
{
	return agentRadius;
}

inline float ResourceNavMesh::GetAgentMaxClimb() const
{
	return agentMaxClimb;
}

inline float ResourceNavMesh::GetAgentMaxSlope() const
{
	return agentMaxSlope;
}

inline float ResourceNavMesh::GetCellSize() const
{
	return cellSize;
}

inline float ResourceNavMesh::GetCellHeight() const
{
	return cellHeight;
}

inline float ResourceNavMesh::GetEdgeMaxError() const
{
	return edgeMaxError;
}

inline void ResourceNavMesh::SetAgentHeight(float agentHeight)
{
	this->agentHeight = agentHeight;
}

inline void ResourceNavMesh::SetAgentRadius(float agentRadius)
{
	this->agentRadius = agentRadius;
}

inline void ResourceNavMesh::SetAgentMaxClimb(float agentMaxClimb)
{
	this->agentMaxClimb = agentMaxClimb;
}

inline void ResourceNavMesh::SetAgentMaxSlope(float agentMaxSlope)
{
	this->agentMaxSlope = agentMaxSlope;
}

inline void ResourceNavMesh::SetCellSize(float cellSize)
{
	this->cellSize = cellSize;
}

inline void ResourceNavMesh::SetCellHeight(float cellHeight)
{
	this->cellHeight = cellHeight;
}

inline void ResourceNavMesh::SetEdgeMaxError(float edgeMaxError)
{
	this->edgeMaxError = edgeMaxError;
}

inline int ResourceNavMesh::GetRegionMinSize() const
{
	return regionMinSize;
}

inline int ResourceNavMesh::GetRegionMergeSize() const
{
	return regionMergeSize;
}

inline int ResourceNavMesh::GetPartitionType() const
{
	return partitionType;
}

inline int ResourceNavMesh::GetEdgeMaxLen() const
{
	return edgeMaxLen;
}

inline int ResourceNavMesh::GetVertsPerPoly() const
{
	return vertsPerPoly;
}

inline int ResourceNavMesh::GetDetailSampleDist() const
{
	return detailSampleDist;
}

inline int ResourceNavMesh::GetDetailSampleMaxError() const
{
	return detailSampleMaxError;
}

inline int ResourceNavMesh::GetTileSize() const
{
	return tileSize;
}

inline int ResourceNavMesh::GetMaxTiles() const
{
	return maxTiles;
}

inline int ResourceNavMesh::GetMaxPolysPerTile() const
{
	return maxPolysPerTile;
}

inline void ResourceNavMesh::SetRegionMinSize(int regionMinSize)
{
	this->regionMinSize = regionMinSize;
}

inline void ResourceNavMesh::SetRegionMergeSize(int regionMergeSize)
{
	this->regionMergeSize = regionMergeSize;
}

inline void ResourceNavMesh::SetPartitionType(int partitionType)
{
	this->partitionType = partitionType;
}

inline void ResourceNavMesh::SetEdgeMaxLen(int edgeMaxLen)
{
	this->edgeMaxLen = edgeMaxLen;
}

inline void ResourceNavMesh::SetVertsPerPoly(int vertsPerPoly)
{
	this->vertsPerPoly = vertsPerPoly;
}

inline void ResourceNavMesh::SetDetailSampleDist(int detailSampleDist)
{
	this->detailSampleDist = detailSampleDist;
}

inline void ResourceNavMesh::SetDetailSampleMaxError(int detailSampleMaxError)
{
	this->detailSampleMaxError = detailSampleMaxError;
}

inline void ResourceNavMesh::SetTileSize(int tileSize)
{
	this->tileSize = tileSize;
}

inline void ResourceNavMesh::SetMaxTiles(int maxTiles)
{
	this->maxTiles = maxTiles;
}

inline void ResourceNavMesh::SetMaxPolysPerTile(int maxPolysPerTile)
{
	this->maxPolysPerTile = maxPolysPerTile;
}

inline bool ResourceNavMesh::GetKeepInterResults() const
{
	return keepInterResults;
}

inline void ResourceNavMesh::SetKeepInterResults(bool keepInterResults)
{
	this->keepInterResults = keepInterResults;
}

inline unsigned char ResourceNavMesh::GetNavMeshDrawFlags() const
{
	return navMeshDrawFlags;
}

inline void ResourceNavMesh::SetNavMeshDrawFlags(unsigned char navMeshDrawFlags)
{
	this->navMeshDrawFlags = navMeshDrawFlags;
}