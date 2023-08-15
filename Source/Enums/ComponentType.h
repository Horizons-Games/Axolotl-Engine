#pragma once

enum class ComponentType
{
	UNKNOWN,
	MESHRENDERER,
	TRANSFORM,
	TRANSFORM2D,
	LIGHT,
	CAMERA,
	CAMERASAMPLE,
	PLAYER,
	ANIMATION,
	CANVAS,
	IMAGE,
	BUTTON,
	SLIDER,
	RIGIDBODY,
	AUDIOSOURCE,
	AUDIOLISTENER,
	MESHCOLLIDER,
	SCRIPT,
	BREAKABLE,
	PARTICLE,
	CUBEMAP,
	TRAIL,
	LINE,
	PLAYERINPUT
};

class ComponentAnimation;
class ComponentAudioListener;
class ComponentAudioSource;
class ComponentBreakable;
class ComponentCamera;
class ComponentCameraSample;
class ComponentCubemap;
class ComponentLight;
class ComponentMeshCollider;
class ComponentMeshRenderer;
class ComponentPlayer;
class ComponentPlayerInput;
class ComponentParticleSystem;
class ComponentRigidBody;
class ComponentScript;
class ComponentTransform;
class ComponentTrail;
class ComponentLine;

class ComponentButton;
class ComponentCanvas;
class ComponentImage;
class ComponentTransform2D;
class ComponentSlider;

template<typename T>
struct ComponentToEnum
{
};

template<>
struct ComponentToEnum<ComponentAnimation>
{
	const static ComponentType value = ComponentType::ANIMATION;
};

template<>
struct ComponentToEnum<ComponentAudioListener>
{
	const static ComponentType value = ComponentType::AUDIOLISTENER;
};

template<>
struct ComponentToEnum<ComponentAudioSource>
{
	const static ComponentType value = ComponentType::AUDIOSOURCE;
};

template<>
struct ComponentToEnum<ComponentBreakable>
{
	const static ComponentType value = ComponentType::BREAKABLE;
};

template<>
struct ComponentToEnum<ComponentCamera>
{
	const static ComponentType value = ComponentType::CAMERA;
};

template<>
struct ComponentToEnum<ComponentCameraSample>
{
	const static ComponentType value = ComponentType::CAMERASAMPLE;
};

template<>
struct ComponentToEnum<ComponentCubemap>
{
	const static ComponentType value = ComponentType::CUBEMAP;
};

template<>
struct ComponentToEnum<ComponentLight>
{
	const static ComponentType value = ComponentType::LIGHT;
};

template<>
struct ComponentToEnum<ComponentMeshCollider>
{
	const static ComponentType value = ComponentType::MESHCOLLIDER;
};

template<>
struct ComponentToEnum<ComponentMeshRenderer>
{
	const static ComponentType value = ComponentType::MESHRENDERER;
};

template<>
struct ComponentToEnum<ComponentPlayer>
{
	const static ComponentType value = ComponentType::PLAYER;
};

template<>
struct ComponentToEnum<ComponentPlayerInput>
{
	const static ComponentType value = ComponentType::PLAYERINPUT;
};

template<>
struct ComponentToEnum<ComponentParticleSystem>
{
	const static ComponentType value = ComponentType::PARTICLE;
};

template<>
struct ComponentToEnum<ComponentRigidBody>
{
	const static ComponentType value = ComponentType::RIGIDBODY;
};

template<>
struct ComponentToEnum<ComponentScript>
{
	const static ComponentType value = ComponentType::SCRIPT;
};

template<>
struct ComponentToEnum<ComponentTransform>
{
	const static ComponentType value = ComponentType::TRANSFORM;
};

template<>
struct ComponentToEnum<ComponentTrail>
{
	const static ComponentType value = ComponentType::TRAIL;
};

template<>
struct ComponentToEnum<ComponentButton>
{
	const static ComponentType value = ComponentType::BUTTON;
};

template<>
struct ComponentToEnum<ComponentCanvas>
{
	const static ComponentType value = ComponentType::CANVAS;
};

template<>
struct ComponentToEnum<ComponentImage>
{
	const static ComponentType value = ComponentType::IMAGE;
};

template<>
struct ComponentToEnum<ComponentTransform2D>
{
	const static ComponentType value = ComponentType::TRANSFORM2D;
};

template<>
struct ComponentToEnum<ComponentSlider>
{
	const static ComponentType value = ComponentType::SLIDER;
};

template<>
struct ComponentToEnum<ComponentLine>
{
	const static ComponentType value = ComponentType::LINE;
};
