#include "StdAfx.h"
#include "LightAttackBullet.h"

LightAttackBullet::LightAttackBullet() :
	Script(),
	parentTransform(nullptr),
	rigidBody(nullptr),
	velocity(15.0f),
	audioSource(nullptr),
	stunTime(10.0f),
	damageAttack(10.0f)
{
}

void LightAttackBullet::Start()
{
}

void LightAttackBullet::Update(float deltaTime)
{
}
