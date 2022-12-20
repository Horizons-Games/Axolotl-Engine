#pragma once

#include <vector>
#include <string>

#include <MathGeoLib/src/Geometry/AABB.h>
#include <MathGeoLib/src/Geometry/OBB.h>

class aiScene;
class Mesh;

class Model
{
public:
	Model();
	~Model();

	void Load(const char* fileName);
	void LoadMaterials(const aiScene* scene);
	void LoadMeshes(const aiScene* scene);
	void Draw();

	const std::string GetDirectory() const;

	const AABB& GetAABB() const;
	const OBB& GetOBB() const;

	int GetNumVertices() const;
	int GetNumTriangles() const;
	unsigned GetTextureId(unsigned idx) const;
	const float3& GetTranslation() const;
	const float3& GetScale() const;
	const float3& GetRotationF3() const;
	const float4x4& GetRotationF4x4() const;

	void SetTranslation(const float3 &translation);
	void SetRotation(const float3 &roation);
	void SetScale(const float3& scale);

private:
	const char* path;

	std::vector<unsigned> textures;
	std::vector<Mesh*> meshes;

	float3 translation;
	float3 scale;
	float3 rotation;
	
	AABB aabb;
	OBB obb;
};

