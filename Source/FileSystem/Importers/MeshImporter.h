#pragma once
#include "FileSystem/Importers/Importer.h"
#include "FileSystem/Data.h"
#include "assimp/mesh.h"


class MeshImporter : public Importer<aiMesh, DataMesh>
{
public:
    MeshImporter() = default;
    ~MeshImporter();

    void Import(const aiMesh* mesh, DataMesh* ourMesh) override;
    uint64_t Save(const DataMesh* ourMesh, char* &fileBuffer) override;
    void Load(const char* fileBuffer, DataMesh* ourMesh) override;

private:
    char* buffer;
};

inline MeshImporter::~MeshImporter()
{
    delete[] buffer;
}

