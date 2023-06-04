#include "EmitterInstance.h"

#include "ParticleEmitter.h"
#include "ParticleModule.h"

#include "GL/glew.h"

#include <random>

EmitterInstance::EmitterInstance(const std::shared_ptr<ParticleEmitter> emitter, ComponentParticleSystem* owner) :
	emitter(emitter), owner(owner), aliveParticles(0), lastEmission(0.0f)
{
	srand(static_cast <unsigned> (time(nullptr))); //seeding the random generation once

	static const float vertices[] = { -0.5f, -0.5f, 0.0f, 
									   0.5f, -0.5f, 0.0f, 
									   0.5f,  0.5f, 0.0f, 
									  -0.5f,  0.5f, 0.0f };

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	static const unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ARRAY_BUFFER, ibo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)(4 * sizeof(float) * 3));

	// Instances
	glGenBuffers(1, &instanceVbo);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);

	unsigned int sizeOfVertex = sizeof(float3) * 4;

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)0);
	glVertexAttribDivisor(3, 1);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)(sizeof(float3)));
	glVertexAttribDivisor(4, 1);

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)(2 * sizeof(float3)));
	glVertexAttribDivisor(5, 1);

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)(3 * sizeof(float3)));
	glVertexAttribDivisor(6, 1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

EmitterInstance::~EmitterInstance()
{
	owner = nullptr;
	particles.clear();

	// Buffer cleanup
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(1, &instanceVbo);
}

void EmitterInstance::Init()
{
	particles.resize(emitter->GetMaxParticles());
	aliveParticles = 0;
	lastEmission = 0.0f;
}

void EmitterInstance::UpdateModules()
{
	std::vector<ParticleModule*> modules = emitter->GetModules();
	
	for (ParticleModule* module : modules)
	{
		module->Update(this);
	}
}

float EmitterInstance::CalculateRandomValueInRange(float min, float max)
{
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

void EmitterInstance::DrawParticles()
{
}

void EmitterInstance::DrawDD()
{
	emitter->DrawDD(this);
}
