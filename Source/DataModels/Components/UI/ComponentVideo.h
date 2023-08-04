#pragma once
#include "Auxiliar/Generics/Drawable.h"
#include "DataModels/Components/Component.h"
#include "Resources/ResourceVideo.h"

class ComponentVideo : public Component, public Drawable
{
public:
	ComponentVideo(bool active, GameObject* owner);
	~ComponentVideo() override;
	void SetVideo(const std::shared_ptr<ResourceVideo>& video);
	std::shared_ptr<ResourceVideo> GetVideo() const;
	void Draw() const override;


private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;
	std::shared_ptr<ResourceVideo> video;
};

inline void ComponentVideo::SetVideo(const std::shared_ptr<ResourceVideo>& video)
{
	this->video = video;
}

inline std::shared_ptr<ResourceVideo> ComponentVideo::GetVideo() const
{
	return this->video;
}