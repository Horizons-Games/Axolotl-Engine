#include "StdAfx.h"
#include "ComponentText2D.h"
#include "FileSystem/ModuleFileSystem.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "FileSystem/ModuleResources.h"
#include "Program/Program.h"
#include "GameObject/GameObject.h"
#include "ComponentCanvas.h"
#include "ComponentTransform2D.h"
#include "Resources/ResourceFont.h"
#include "Defines/ExtensionDefines.h"
#include "Defines/FileSystemDefines.h"
#include <GL/glew.h>

ComponentText2D::ComponentText2D(bool active, GameObject* owner) 
	:
	Component(ComponentType::TEXT2D, active, owner, true)	
{	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	color = float3(0.5f, 0.8f, 0.2f);
	programShared = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::TEXT_SHADER);		
}

ComponentText2D::~ComponentText2D() 
{
	DeleteVAOandVBO();		
}

void ComponentText2D::Init()
{	
}

void ComponentText2D::Draw() const 
{			
	if (currentFont!=nullptr)
	{		
		if ( text != "")
		{
			if (currentFont->IsCharacterEmpty())
			{
				currentFont->LoadCharacter();
			}			
			GLint programinuse;

			if (programShared->IsValidProgram())
			{
				programShared->Activate();
				ComponentTransform2D* transform = GetOwner()->GetComponentInternal<ComponentTransform2D>();

				const float4x4& proj = App->GetModule<ModuleCamera>()->GetOrthoProjectionMatrix();
				const float4x4& model = transform->GetGlobalScaledMatrix();
				float4x4 view = float4x4::identity;

				float3 pos = transform->GetGlobalPosition();

				ComponentCanvas* canvas = transform->WhichCanvasContainsMe();
				float factor = 0.5f;
				if (canvas)
				{
					canvas->RecalculateSizeAndScreenFactor();
					factor = canvas->GetScreenFactor();
					view = view * float4x4::Scale(factor, factor, factor);
				}
											
				programShared->BindUniformFloat4x4("view", reinterpret_cast<const float*>(&view), true);
				programShared->BindUniformFloat4x4("model", reinterpret_cast<const float*>(&model), true);
				programShared->BindUniformFloat4x4("proj", reinterpret_cast<const float*>(&proj), true);

				glGetIntegerv(GL_CURRENT_PROGRAM, &programinuse);

				
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				RenderText(text,
						   0.0f,
						   0.0f,
						   0.6f,
						   color,
						   currentFont->GetCharacters());
				programShared->Deactivate();
			}
		}
	}
}

void ComponentText2D::InternalSave(Json& meta)
{	
	UID uidFont = 0;
	std::string assetPath = "";
	if (currentFont != nullptr)
	{
		uidFont = currentFont->GetUID();
		assetPath=currentFont->GetAssetsPath();
	}
	meta["fontUID"] = static_cast<UID>(uidFont);
	meta["assetPathImage"] = assetPath.c_str();
	meta["text"] = text.c_str();
	meta["color_x"] = static_cast<float>(color.x);
	meta["color_y"] = static_cast<float>(color.y);
	meta["color_z"] = static_cast<float>(color.z);	
}

void ComponentText2D::InternalLoad(const Json& meta)
{	
#ifdef ENGINE
	std::string path = meta["assetPathImage"];
	bool resourceExists = !path.empty() && App->GetModule<ModuleFileSystem>()->Exists(path.c_str());
	if (resourceExists)
	{
		std::shared_ptr<ResourceFont> resourceFont =
			App->GetModule<ModuleResources>()->RequestResource<ResourceFont>(path);
		if (resourceFont)
		{
			currentFont = resourceFont;
		}
	}
#else
	UID uidFont = meta["fontUID"];
	std::shared_ptr<ResourceFont> resourceFont =
		App->GetModule<ModuleResources>()->SearchResource<ResourceFont>(uidFont);
	if (resourceFont)
	{
		currentFont = resourceFont;
	}
#endif
	text = meta["text"];
	color.x = static_cast<float>(meta["color_x"]);
	color.y = static_cast<float>(meta["color_y"]);
	color.z = static_cast<float>(meta["color_z"]);
}

std::vector<std::shared_ptr<ResourceFont>> ComponentText2D::GetFonts() const
{
	std::vector<std::string> paths = App->GetModule<ModuleFileSystem>()->ListFiles(FONT_PATH);
	std::vector<std::shared_ptr<ResourceFont>> fonts;
	for (std::string path : paths)
	{
		std::size_t found = path.find(".meta");
		if (found != std::string::npos)
		{
			ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
			char* metaBuffer = {};
			std::string assetPath = FONT_PATH + path;
			fileSystem->Load(assetPath.c_str(), metaBuffer);
			rapidjson::Document doc;
			Json meta(doc, doc);
			meta.fromBuffer(metaBuffer);
			delete metaBuffer;
			UID uid = (UID) meta["UID"];
			std::string libpath = FONT_LIB_PATH +std::to_string(uid)+GENERAL_BINARY_EXTENSION;	
			
			if (fileSystem->Exists(libpath.c_str()))
			{
				path.erase(path.begin() + found, path.end());
				fonts.push_back(App->GetModule<ModuleResources>()->RequestResource<ResourceFont>(path));
			}
			
		}
	}
	return fonts;
}

void ComponentText2D::CreateVAO()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ComponentText2D::DeleteVAOandVBO()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void ComponentText2D::RenderText(const std::string &text,
								 float x,
								 float y,
								 float scale,
								 const float3 &color,
								 std::map<char, Character> characters) const
{	
	programShared->BindUniformInt("text", 0);
	programShared->BindUniformFloat3("textColor", color);	
	
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	/*ENGINE_LOG("VAO: %i", VAO)*/
	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
		
		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;		
		// update VBO for each character
		float vertices[6][4] = { { xpos / 48.0f, (ypos + h) / 48.0f, 1.0f, 1.0f },
								 { xpos / 48.0f, ypos / 48.0f, 1.0f, 0.0f },
								 { (xpos + w) / 48.0f, ypos / 48.0f, 0.0f, 0.0f },

			{ xpos / 48.0f, (ypos + h) / 48.0f, 1.0f, 1.0f },
								 { (xpos + w) / 48.0f, ypos / 48.0f, 0.0f, 0.0f },
								 { (xpos + w) / 48.0f, (ypos + h) / 48.0f, 0.0f, 1.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::string error;
			switch (err)
			{
				case GL_INVALID_OPERATION:
					error = "INVALID_OPERATION";
					break;
				case GL_INVALID_ENUM:
					error = "INVALID_ENUM";
					break;
				case GL_INVALID_VALUE:
					error = "INVALID_VALUE";
					break;
				case GL_OUT_OF_MEMORY:
					error = "OUT_OF_MEMORY";
					break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					error = "INVALID_FRAMEBUFFER_OPERATION";
					break;
			}
			LOG_INFO(error);
		}
		/*for (int i=0;i<6;i++)
		{
			ENGINE_LOG("vertex matrix row %i: %1.5f %1.5f",i+1, vertices[i][0], vertices[i][1])
		}	*/	
		/*ENGINE_LOG("id texture: %i", ch.TextureID)*/
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		/*ENGINE_LOG("VBO: %i", VBO)*/
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	//programShared->Deactivate();
}

void ComponentText2D::SetCharacters(unsigned int item)
{
	
	std::vector<std::shared_ptr<ResourceFont>> resourceFonts = GetFonts();
	currentFont = resourceFonts[item];
	currentFont->LoadCharacter();
}

void ComponentText2D::Update()
{	
}