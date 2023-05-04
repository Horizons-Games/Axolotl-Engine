#include "EmitterInstance.h"

EmitterInstance::EmitterInstance(ParticleEmitter* emitter, ComponentParticleSystem* owner) :
	emitter(emitter), owner(owner)
{
}

EmitterInstance::~EmitterInstance()
{
	delete emitter;
	delete owner;

	particles.clear();
}

void EmitterInstance::SetShape(ShapeType shape)
{
	this->shape = shape;
}
