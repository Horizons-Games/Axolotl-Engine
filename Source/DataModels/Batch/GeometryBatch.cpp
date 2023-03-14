#include "GeometryBatch.h"

#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentTransform.h"
#include "GameObject/GameObject.h"
#include "Resources/ResourceMesh.h"
#include "DataModels/Batch/BatchFlags.h"

#include <gl/glew.h>



GeometryBatch::GeometryBatch()
{
	buffers = new ResourceMesh(0,"","","");
	
	CreateVBO();
	CreateEBO();
	CreateVAO();
}

GeometryBatch::~GeometryBatch()
{
	components.clear();
	resourceMeshes.clear();
	CleanUp();
}

void GeometryBatch::AddComponentMeshRenderer(ComponentMeshRenderer* newComponent)
{
	if (newComponent != nullptr)
	{
		if (components.empty())
		{
			if (!newComponent->GetMesh()->GetVertices().empty())
				flags |= HAS_VERTICES;

			if(!newComponent->GetMesh()->GetNormals().empty())
				flags |= HAS_NORMALS;

			if (!newComponent->GetMesh()->GetTextureCoords().empty())
				flags |= HAS_TEXTURE_COORDINATES;

			if (!newComponent->GetMesh()->GetTangents().empty())
				flags |= HAS_TANGENTS;
		}

		AddUniqueComponent(newComponent->GetMesh().get());
		components.push_back(newComponent);
	}
}

void GeometryBatch::CreateVBO()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	unsigned count = 1;
	unsigned padding = 4 * sizeof(float);
	unsigned offset = 0;

	for (ResourceMesh* resourceMesh : resourceMeshes)
	{
		offset = padding * count;
		//position			//uv				//normal		
		unsigned vertexSize = (sizeof(float) * 3 + sizeof(float) * 2 + sizeof(float) * 3);
		//tangents
		if (resourceMesh->GetTangents().size() != 0)
		{
			vertexSize += sizeof(float) * 3;
		}
		//unsigned vertexSize = (sizeof(float) * 3 + sizeof(float) * 2);
		GLuint bufferSize = vertexSize * resourceMesh->GetNumVertices();

		glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

		GLuint positionSize = offset * 3 * resourceMesh->GetNumVertices();


		glBufferSubData(GL_ARRAY_BUFFER, offset, positionSize, &(resourceMesh->GetVertices()[0]));

		GLuint uvOffset = positionSize;
		GLuint uvSize = offset * 2 * resourceMesh->GetNumVertices();

		float2* uvs = (float2*)(glMapBufferRange(GL_ARRAY_BUFFER, uvOffset, uvSize, GL_MAP_WRITE_BIT));

		for (unsigned int i = 0; i < resourceMesh->GetNumVertices(); ++i)
		{
			uvs[i] = float2(resourceMesh->GetTextureCoords()[i].x, resourceMesh->GetTextureCoords()[i].y);
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);

		unsigned normalsOffset = positionSize + uvSize;
		unsigned normalsSize = offset * 3 * resourceMesh->GetNumVertices();
		glBufferSubData(GL_ARRAY_BUFFER, normalsOffset, normalsSize, &resourceMesh->GetNormals()[0]);

		if (resourceMesh->GetTangents().size() != 0)
		{
			unsigned tangentsOffset = positionSize + uvSize + normalsSize;
			unsigned tangentsSize = offset * 3 * resourceMesh->GetNumVertices();
			glBufferSubData(GL_ARRAY_BUFFER, tangentsOffset, tangentsSize, &resourceMesh->GetTangents()[0]);
		}
		count++;
	}
	

}

void GeometryBatch::CreateEBO()
{
	for (ResourceMesh* resourceMesh : resourceMeshes)
	{
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		GLuint indexSize = sizeof(GLuint) * resourceMesh->GetNumFaces() * 3;

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, nullptr, GL_STATIC_DRAW);

		GLuint* indices = (GLuint*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

		for (unsigned int i = 0; i < resourceMesh->GetNumFaces(); ++i)
		{
			assert(resourceMesh->GetFacesIndices()[i].size() == 3); // note: assume triangles = 3 indices per face
			*(indices++) = resourceMesh->GetFacesIndices()[i][0];
			*(indices++) = resourceMesh->GetFacesIndices()[i][1];
			*(indices++) = resourceMesh->GetFacesIndices()[i][2];
		}
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	}
}


void GeometryBatch::BindBatch()
{
	// Set up the vertex data
	float vertices[] = {
		// First triangle
		-0.5f, - 0.5f, 0.0f,  // Botton left
		-0.5f, 0.5f, 0.0f,  // Top left
		 0.5f, - 0.5f, 0.0f,  // Bottom right
		 0.5f, 0.5f, 0.0f  // top right
	};

	// Set up the index data
	GLuint indices[] = {
		0, 2, 3, // First triangle
		0, 3, 1, // Second triangle
	};

	// Create the vertex buffer and load the vertex data into it
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	// Create the index buffer and load the index data into it
	GLuint indexBuffer;
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

	// Set up the vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	// Create the draw commands buffer
	Command drawCommands[] = {
		{3, 1, 0, 0, 0}, // First triangle
		{3, 1, 3, 0, 0}, // Second triangle
	};

	// Create the draw commands buffer and load the draw commands into it
	glGenBuffers(1, &indirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(drawCommands), drawCommands, GL_DYNAMIC_DRAW);

}

void GeometryBatch::UpdateCommands()
{
	resourceMeshIndex = 0;
	commands.clear();
	commands.reserve(components.size());//need to verify the size if it's matching with uniqueComponent
	unsigned indices_offset = 0;
	unsigned vertices_offset = 0;
	for (ResourceMesh* resourceMesh : resourceMeshes)
	{

		//do a for for all the instaces existing
		unsigned size = resourceMeshes.size();
		Command newCommand = { resourceMesh->GetNumIndexes(), 1, indices_offset,vertices_offset/3, resourceMeshIndex };
		commands.push_back(newCommand);
		indices_offset += size;
		vertices_offset += resourceMesh->GetNumVertices();
		resourceMeshIndex++;
	}

	//send to gpu
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(Command) * commands.size(), &commands.front(), GL_DYNAMIC_DRAW);

}

void GeometryBatch::CreateVAO()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//verify which data to send in buffer

	//vertices
	glGenBuffers(1, &buffers->instance[0]);
	glBindBuffer(GL_ARRAY_BUFFER,buffers->instance[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	//texture
	glGenVertexArrays(1, &buffers->instance[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[1]);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
	//normals
	glGenVertexArrays(1, &buffers->instance[2]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[2]);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	//tangents
	glGenVertexArrays(1, &buffers->instance[3]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[3]);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));

	glGenBuffers(1, &indirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);

	glBindVertexArray(0);

}

void GeometryBatch::UpdateVAO()
{
	glBindVertexArray(vao);

	// create buffer + initialise it
	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[0]);
	glBufferData(GL_ARRAY_BUFFER, 300 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[1]);
	glBufferData(GL_ARRAY_BUFFER, 300 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[2]);
	glBufferData(GL_ARRAY_BUFFER, 300 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[3]);
	glBufferData(GL_ARRAY_BUFFER, 300 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

	// Feed buffer with glBufferSubData
	int offset = 0;
	for (ResourceMesh* resourceMesh : resourceMeshes)
	{
		//Vertices
		glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[0]);
		glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(float), resourceMesh->GetNumVertices() * sizeof(float), &resourceMesh->GetVertices());

		//Normals
		glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[1]);
		glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(float), resourceMesh->GetNumVertices() * sizeof(float), &resourceMesh->GetNormals());

		//Texture
		glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[2]);
		glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(float), resourceMesh->GetNumVertices() * sizeof(float), &resourceMesh->GetTextureCoords());

		//Tangents
		glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[3]);
		glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(float), resourceMesh->GetNumVertices() * sizeof(float), &resourceMesh->GetTangents());

		offset += resourceMesh->GetNumVertices();
	}
	glBindVertexArray(0);
}

void GeometryBatch::AddUniqueComponent(ResourceMesh* resourceMesh)
{
	if (isUniqueResourceMesh(resourceMesh))
	{
		resourceMeshes.push_back(resourceMesh);
	}
}

const GameObject* GeometryBatch::GetComponentOwner(const ResourceMesh* resourceMesh)
{
	for (ComponentMeshRenderer* component : components)
	{
		if (component->GetMesh().get() == resourceMesh)
		{
			return component->GetOwner();
		}
	}

	return nullptr;
}

bool GeometryBatch::isUniqueResourceMesh(const ResourceMesh* resourceMesh)
{
	for (ResourceMesh* resourceMeshe : resourceMeshes)
	{
		if (resourceMeshe == resourceMesh)
		{
			return false;
		}
	}

	return true;
}

bool GeometryBatch::CleanUp()
{
	glDeleteBuffers(1,&vao);
	glDeleteBuffers(1,&ebo);
	glDeleteBuffers(1,&vbo);
	glDeleteBuffers(1, &indirectBuffer);

	return true;
}
