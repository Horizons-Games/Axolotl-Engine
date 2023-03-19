#pragma once

#include "Resource.h"

#include "Components/ComponentMeshRenderer.h"

#include "Math/float4x4.h"

#include <vector>
#include <memory>

class ResourceMesh;
class ResourceMaterial;

struct OptionsModel
{
};

class ResourceModel : virtual public Resource
{
public:
	struct Node
	{
		char* name;
		float4x4 transform;
		int parent;
		std::vector<std::pair<std::shared_ptr<ResourceMesh>, std::shared_ptr<ResourceMaterial>>> meshRenderers;
	};

public:
	ResourceModel(UID resourceUID, 
		const std::string& fileName, 
		const std::string& assetsPath, 
		const std::string& libraryPath);
	virtual ~ResourceModel() override;

	ResourceType GetType() const override;

	void SaveImporterOptions(Json& meta) override {};
	void LoadImporterOptions(Json& meta) override {};

	const size_t GetNumNodes() const;
	const std::vector<Node*>& GetNodes() const;

	OptionsModel& GetOptions();

	void SetNodes(const std::vector<Node*>& nodes);

	void AppendNode(Node* node);

protected:
	void InternalLoad() override;
	void InternalUnload() override;

private:
	OptionsModel options;

	std::vector<Node*> nodes;
};

inline ResourceType ResourceModel::GetType() const
{
	return ResourceType::Model;
}

inline const size_t ResourceModel::GetNumNodes() const
{
	return nodes.size();
}

inline const std::vector<ResourceModel::Node*>& ResourceModel::GetNodes() const
{
	return nodes;
}

inline OptionsModel& ResourceModel::GetOptions()
{
	return options;
}

inline void ResourceModel::SetNodes(const std::vector<Node*>& nodes)
{
	this->nodes = nodes;
}

inline void ResourceModel::AppendNode(Node* node)
{
	nodes.push_back(node);
}