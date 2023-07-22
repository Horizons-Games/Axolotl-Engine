#pragma once
#include "Component.h"

#include "Auxiliar/Generics/Updatable.h"
#include "Trail/Trail.h"

class ImGradient;
class ResourceTexture;

class ComponentTrail : public Component, public Updatable
{
public:
	ComponentTrail(bool active, GameObject* owner);
	~ComponentTrail();

	void Update() override;
	void Render() override;

	const float GetDuration() const;
	void SetDuration(float duration);

	const float GetMinDistance() const;
	void SetMinDistance(float minDistance);

	const float GetWidth() const;
	void SetWidth(float width);

	std::shared_ptr<ResourceTexture> GetTexture() const;
	void SetTexture(const std::shared_ptr<ResourceTexture>& texture);

	ImGradient* GetGradient();
	void SetGradient(ImGradient* gradient);
	
	bool IsOnPlay();
	void SetOnPlay(bool onPlay);

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;
	std::shared_ptr<ResourceTexture> texture;

	Trail* trail;
};

inline const float ComponentTrail::GetDuration() const
{
	return trail->GetDuration();
}

inline void ComponentTrail::SetDuration(float duration)
{
	trail->SetDuration(duration);
}

inline const float ComponentTrail::GetMinDistance() const
{
	return trail->GetMinDistance();
}

inline void ComponentTrail::SetMinDistance(float minDistance)
{
	trail->SetMinDistance(minDistance);
}

inline const float ComponentTrail::GetWidth() const
{
	return trail->GetWidth();
}

inline void ComponentTrail::SetWidth(float width)
{
	trail->SetWidth(width);
}

inline std::shared_ptr<ResourceTexture> ComponentTrail::GetTexture() const
{
	return trail->GetTexture();
}

inline void ComponentTrail::SetTexture(const std::shared_ptr<ResourceTexture>& texture)
{
	trail->SetTexture(texture);
}

inline ImGradient* ComponentTrail::GetGradient()
{
	return trail->GetGradient();
}

inline void ComponentTrail::SetGradient(ImGradient* gradient)
{
	trail->SetGradient(gradient);
}

inline bool ComponentTrail::IsOnPlay()
{
	return trail->IsOnPlay();
}

inline void ComponentTrail::SetOnPlay(bool onPlay)
{
	trail->SetOnPlay(onPlay);
}
