#pragma once

enum class ComponentType
{
	UNKNOWN,
	MESHRENDERER,
	TRANSFORM,
	TRANSFORM2D,
	LIGHT,
	CAMERA,
	PLAYER,
	ANIMATION,
	CANVAS,
	IMAGE,
	BUTTON,
	RIGIDBODY,
	MOCKSTATE,
	AUDIOSOURCE,
	AUDIOLISTENER,
	MESHCOLLIDER,
	SCRIPT
};

class ComponentAnimation;
class ComponentAudioListener;
class ComponentAudioSource;
class ComponentCamera;
class ComponentLight;
class ComponentMeshColider;
class ComponentMeshRenderer;
class ComponentMockState;
class ComponentPlayer;
class ComponentRigidBody;
class ComponentScript;
class ComponentTransform;

class ComponentButton;
class ComponentCanvas;
class ComponentImage;
class ComponentTransform2D;

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
struct ComponentToEnum<ComponentCamera>
{
	const static ComponentType value = ComponentType::CAMERA;
};

template<>
struct ComponentToEnum<ComponentLight>
{
	const static ComponentType value = ComponentType::LIGHT;
};

template<>
struct ComponentToEnum<ComponentMeshColider>
{
	const static ComponentType value = ComponentType::MESHCOLLIDER;
};

template<>
struct ComponentToEnum<ComponentMeshRenderer>
{
	const static ComponentType value = ComponentType::MESHRENDERER;
};

template<>
struct ComponentToEnum<ComponentMockState>
{
	const static ComponentType value = ComponentType::MOCKSTATE;
};

template<>
struct ComponentToEnum<ComponentPlayer>
{
	const static ComponentType value = ComponentType::PLAYER;
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
