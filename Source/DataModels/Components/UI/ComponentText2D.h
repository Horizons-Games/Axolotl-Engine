#pragma once
#include "Components/Component.h"
#include "Auxiliar/Generics/Drawable.h"
#include "Auxiliar/Generics/Updatable.h"


class ResourceFont;
class ComponentFont;
class Program;
struct Character;


class ComponentText2D : public Component, public Updatable, public Drawable
{
public:
	ComponentText2D(bool active, GameObject* owner);
	~ComponentText2D() override;
	void Init();
	void Update();

	
	void InternalSave(Json& meta) override; 
	void InternalLoad(const Json& meta) override;
	void Draw() const override;
	/*void SetFontNumber(unsigned int fontNumber);*/	
	std::vector<std::shared_ptr<ResourceFont>> getFonts();
	void SetText(std::string text);
	void SetColor(float3 color);
	void SetCharacters(unsigned int item);

private:
	
	void CreateVAO();
	void DeleteVAOandVBO();
	void RenderText(std::string text,float x,float y, float scale, float3 color, std::map<char, Character> characters) const;
	Program* programShared;
	unsigned int VAO;
	unsigned int VBO;
	std::string text;		
	std::shared_ptr<ResourceFont> currentFont;
	float3 color;
	/*unsigned int fontNumber; */
	
};

inline void ComponentText2D::SetText(std::string text) 
{
	this->text = text;
}
inline void ComponentText2D::SetColor(float3 color)
{
	this->color = color;
}