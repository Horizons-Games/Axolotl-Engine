#pragma once

#include <vector>
#include <memory>

class GeometryBatch;

class BatchManager
{
public:
	BatchManager();
	~BatchManager();
private:
	std::vector<std::shared_ptr<GeometryBatch>> geometryBatches;
};