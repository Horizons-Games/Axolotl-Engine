#pragma once
#include "FileSystem/Importers/Importer.h"
#include "DataModels/Resources/ResourceStateMachine.h"

class StateMachineImporter : public Importer<ResourceStateMachine>
{
public:
    StateMachineImporter();
    ~StateMachineImporter() override;

    void Import(const char* filePath, std::shared_ptr<ResourceStateMachine> resource) override;
    void Load(const char* fileBuffer, std::shared_ptr<ResourceStateMachine> resource) override;
    void Save(const std::shared_ptr<ResourceStateMachine>& resource, char*& fileBuffer, unsigned int& size) override;

protected:
};
