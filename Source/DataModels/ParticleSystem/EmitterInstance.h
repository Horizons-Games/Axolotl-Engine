#pragma once

#include "Math/float3.h"
#include "Math/Quat.h"

#include <vector>

class ParticleEmitter;
class ComponentParticleSystem;

class EmitterInstance
{
public:
	struct Particle
	{
		float4 velocity;
		float3 position;
		float3 size;
		float3 color;
		Quat rotation;

		float lifetime;
		float distanceToCamera;
	};

	enum class ShapeType { CIRCLE, CONE, BOX };

public:
	EmitterInstance(ParticleEmitter* emitter, ComponentParticleSystem* owner);
	~EmitterInstance();

	void Init();
	void UpdateModules();
	void DrawParticles();

	void SetShape(ShapeType shape);

private:
	std::vector<Particle> particles;
	ParticleEmitter* emitter;
	ComponentParticleSystem* owner;
	ShapeType shape;
};

