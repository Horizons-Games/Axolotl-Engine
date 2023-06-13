#include "WindowComponentParticle.h"

#include "Components/ComponentParticleSystem.h"

#include "ParticleSystem/EmitterInstance.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "ParticleSystem/ParticleModule.h"

WindowComponentParticle::WindowComponentParticle(ComponentParticleSystem* component) :
	ComponentWindow("Particle System", component)
{
}

WindowComponentParticle::~WindowComponentParticle()
{
}

void WindowComponentParticle::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	if (!component)
	{
		return;
	}

	ImGui::Text("");

	ComponentParticleSystem* component = static_cast<ComponentParticleSystem*>(this->component);
	
	int id = 0;
	
	if (component->GetEmitters().size() > 0)
	{
		ImGui::Separator();

		if (ImGui::ArrowButton("##Play", ImGuiDir_Right))
		{
			if (!component->IsPlaying())
			{
				component->Play();
			}
		}
		ImGui::SameLine();

		if (ImGui::Button("||"))
		{
			if (component->IsPlaying())
			{
				component->Stop();
			}
		}

		ImGui::Separator();
	}

	for (EmitterInstance* instance : component->GetEmitters())
	{
		DrawEmitter(instance, id);
		++id;
	}

	if (ImGui::Button("Add an Emitter"))
	{
		std::shared_ptr<ParticleEmitter> emitter = std::make_shared<ParticleEmitter>();
		std::string name = "DefaultEmitter_" + std::to_string(component->GetEmitters().size());
		
		emitter->SetName(&name[0]);
		component->CreateEmitterInstance(emitter);
	}
}

void WindowComponentParticle::DrawEmitter(EmitterInstance* instance, int id)
{
	std::shared_ptr<ParticleEmitter> emitter = instance->GetEmitter();

	ImGui::PushID(id);

	if (emitter)
	{
		ImGui::Dummy(ImVec2(0.0f, 2.5f));
		ImGui::Text(emitter->GetName());

		bool open = emitter->IsVisibleConfig();

		ParticleEmitter::ShapeType shape = emitter->GetShape();

		const char* items[] = { "CIRCLE", "CONE", "BOX" };

		static const char* currentItem;
		switch (shape)
		{
		case ParticleEmitter::ShapeType::CIRCLE:
			currentItem = items[0];
			break;
		case ParticleEmitter::ShapeType::CONE:
			currentItem = items[1];
			break;
		case ParticleEmitter::ShapeType::BOX:
			currentItem = items[2];
			break;
		}

		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		if (ImGui::BeginTable("ParametersTable", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Shape");
			/*ImGui::SameLine(0.0f, 10.0f);*/
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(120.0f);

			if (ImGui::BeginCombo("##shapeCombo", currentItem))
			{
				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					// You can store your selection however you want, outside or inside your objects
					bool isSelected = (currentItem == items[n]);
					if (ImGui::Selectable(items[n], isSelected))
					{
						currentItem = items[n];
						if (isSelected)
						{
							// You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
							ImGui::SetItemDefaultFocus();
						}
					}
				}

				if (currentItem == items[0])
				{
					shape = ParticleEmitter::ShapeType::CIRCLE;
				}
				else if (currentItem == items[1])
				{
					shape = ParticleEmitter::ShapeType::CONE;
				}
				else if (currentItem == items[2])
				{
					shape = ParticleEmitter::ShapeType::BOX;
				}

				emitter->SetShape(shape);

				ImGui::EndCombo();
			}

			int maxParticles = emitter->GetMaxParticles();
			float angle = emitter->GetAngle();
			float radius = emitter->GetRadius();
			float duration = emitter->GetDuration();
			float2 lifespanRange = emitter->GetLifespanRange();
			float2 speedRange = emitter->GetSpeedRange();
			float2 sizeRange = emitter->GetSizeRange();
			float2 rotRange = emitter->GetRotationRange();
			float2 gravRange = emitter->GetGravityRange();
			float4 color = emitter->GetColor();

			bool randomLife = emitter->IsRandomLife();
			bool randomSpeed = emitter->IsRandomSpeed();
			bool randomSize = emitter->IsRandomSize();
			bool randomRot = emitter->IsRandomRot();
			bool randomGrav = emitter->IsRandomGravity();
			bool lifespanModif = false;
			bool speedModif = false;
			bool sizeModif = false;
			bool rotModif = false;
			bool gravModif = false;

			ImGui::TableNextColumn();
			ImGui::Text("Shape config");
			ImGui::TableNextColumn();
			ImGui::Text("Radius:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(60.0f);
			if (ImGui::DragFloat("##radius", &radius, 0.1f, MIN_RADIUS, MAX_RADIUS, "%.2f"))
			{
				if (radius > MAX_RADIUS)
				{
					radius = MAX_RADIUS;
				}
				else if (radius < 0.01)
				{
					radius = 0.01;
				}
				emitter->SetRadius(radius);
			}
			if (shape == ParticleEmitter::ShapeType::CONE)
			{
				ImGui::SameLine();
				ImGui::Text("Angle:"); ImGui::SameLine();
				ImGui::SetNextItemWidth(60.0f);
				if (ImGui::SliderFloat("##angle", &angle, 0.0f, 89.99f, "%.2f"))
				{
					emitter->SetAngle(angle);
				}
			}

			ImGui::Dummy(ImVec2(0.0f, 2.5f));

			ImGui::TableNextColumn();
			ImGui::Text("Max particles");
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(165.0f);
			if (ImGui::InputInt("##maxParticles", &maxParticles, 1, 5))
			{
				if (maxParticles > MAX_PARTICLES)
				{
					maxParticles = MAX_PARTICLES;
				}
				else if (maxParticles < 0)
				{
					maxParticles = 0;
				}
				emitter->SetMaxParticles(maxParticles);
				instance->Init();
			}

			ImGui::TableNextColumn();
			ImGui::Text("Duration");
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(165.0f);
			if (ImGui::InputFloat("##duration", &duration, 1, 5, "%.2f"))
			{
				if (duration > MAX_DURATION)
				{
					duration = MAX_DURATION;
				}
				else if (duration < 0.0f)
				{
					duration = 0.0f;
				}
				emitter->SetDuration(duration);
			}

			ImGui::TableNextColumn();
			ImGui::Text("Lifespan");
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(165.0f);
			if (randomLife)
			{
				if (ImGui::SliderFloat2("##sliderlife", &lifespanRange[0], 0.0f, MAX_DURATION, "%.2f"))
				{
					if (lifespanRange.x > lifespanRange.y)
					{
						lifespanRange.x = lifespanRange.y;
					}
					else if (lifespanRange.x < 0)
					{
						lifespanRange.x = 0;
					}

					if (lifespanRange.y < lifespanRange.x)
					{
						lifespanRange.y = lifespanRange.x;
					}
					else if (lifespanRange.y > MAX_DURATION)
					{
						lifespanRange.y = MAX_DURATION;
					}

					lifespanModif = true;
				}
			}
			else
			{
				if (ImGui::InputFloat("##inputLife", &lifespanRange.x, 1, 5, "%.2f"))
				{
					if (lifespanRange.x > MAX_DURATION)
					{
						lifespanRange.x = MAX_DURATION;
					}
					else if (lifespanRange.x < 0)
					{
						lifespanRange.x = 0;
					}
					lifespanModif = true;
				}
			}
			ImGui::SameLine(0.0f, 5.0f);
			ImGui::Text("Random");
			ImGui::SameLine(0.0f, 5.0f);
			if (ImGui::Checkbox("##randomLife", &randomLife))
			{
				emitter->SetRandomLife(randomLife);
			}

			ImGui::TableNextColumn();
			ImGui::Text("Speed");
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(165.0f);
			if (randomSpeed)
			{
				if (ImGui::SliderFloat2("##sliderSpeed", &speedRange[0], 0.0f, MAX_SPEED, "%.2f"))
				{
					if (speedRange.x > speedRange.y)
					{
						speedRange.x = speedRange.y;
					}
					else if (speedRange.x < 0)
					{
						speedRange.x = 0;
					}

					if (speedRange.y < speedRange.x)
					{
						speedRange.y = speedRange.x;
					}
					else if (speedRange.y > MAX_SPEED)
					{
						speedRange.y = MAX_SPEED;
					}

					speedModif = true;
				}
			}
			else
			{
				if (ImGui::InputFloat("##inputSpeed", &speedRange.x, 1, 5, "%.2f"))
				{
					if (speedRange.x > MAX_SPEED)
					{
						speedRange.x = MAX_SPEED;
					}
					else if (speedRange.x < 0)
					{
						speedRange.x = 0;
					}
					speedModif = true;
				}
			}
			ImGui::SameLine(0.0f, 5.0f);
			ImGui::Text("Random");
			ImGui::SameLine(0.0f, 5.0f);
			if (ImGui::Checkbox("##randomSpeed", &randomSpeed))
			{
				emitter->SetRandomSpeed(randomSpeed);
			}

			ImGui::TableNextColumn();
			ImGui::Text("Size");
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(165.0f);
			if (randomSize)
			{
				if (ImGui::SliderFloat2("##sliderSize", &sizeRange[0], 0.0f, MAX_SIZE, "%.2f"))
				{
					if (sizeRange.x > sizeRange.y)
					{
						sizeRange.x = sizeRange.y;
					}
					else if (sizeRange.x < 0)
					{
						sizeRange.x = 0;
					}

					if (sizeRange.y < sizeRange.x)
					{
						sizeRange.y = sizeRange.x;
					}
					else if (sizeRange.y > MAX_SIZE)
					{
						sizeRange.y = MAX_SIZE;
					}

					sizeModif = true;
				}
			}
			else
			{
				if (ImGui::InputFloat("##inputSize", &sizeRange.x, 1, 5, "%.2f"))
				{
					if (sizeRange.x > MAX_SIZE)
					{
						sizeRange.x = MAX_SIZE;
					}
					else if (sizeRange.x < 0)
					{
						sizeRange.x = 0;
					}

					sizeModif = true;
				}
			}
			ImGui::SameLine(0.0f, 5.0f);
			ImGui::Text("Random");
			ImGui::SameLine(0.0f, 5.0f);
			if (ImGui::Checkbox("##randomSize", &randomSize))
			{
				emitter->SetRandomSize(randomSize);
			}

			ImGui::TableNextColumn();
			ImGui::Text("Rotation");
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(165.0f);
			if (randomRot)
			{
				if (ImGui::SliderFloat2("##sliderRot", &rotRange[0], 0.0f, MAX_ROTATION, "%.2f"))
				{
					if (rotRange.x > rotRange.y)
					{
						rotRange.x = rotRange.y;
					}
					else if (rotRange.x < 0)
					{
						rotRange.x = 0;
					}

					if (rotRange.y < rotRange.x)
					{
						rotRange.y = rotRange.x;
					}
					else if (rotRange.y > MAX_ROTATION)
					{
						rotRange.y = MAX_ROTATION;
					}

					rotModif = true;
				}
			}
			else
			{
				if (ImGui::InputFloat("##inputRot", &rotRange.x, 1, 5, "%.2f"))
				{
					if (rotRange.x > MAX_ROTATION)
					{
						rotRange.x = MAX_ROTATION;
					}
					else if (rotRange.x < 0)
					{
						rotRange.x = 0;
					}

					rotModif = true;
				}
			}
			ImGui::SameLine(0.0f, 5.0f);
			ImGui::Text("Random");
			ImGui::SameLine(0.0f, 5.0f);
			if (ImGui::Checkbox("##randomRot", &randomRot))
			{
				emitter->SetRandomRotation(randomRot);
			}

			ImGui::TableNextColumn();
			ImGui::Text("Gravity");
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(165.0f);
			if (randomGrav)
			{
				if (ImGui::SliderFloat2("##sliderGrav", &gravRange[0], 0.0f, MAX_GRAVITY, "%.2f"))
				{
					if (gravRange.x > gravRange.y)
					{
						gravRange.x = gravRange.y;
					}
					else if (gravRange.x < 0)
					{
						gravRange.x = 0;
					}

					if (gravRange.y < gravRange.x)
					{
						gravRange.y = gravRange.x;
					}
					else if (gravRange.y > MAX_GRAVITY)
					{
						gravRange.y = MAX_GRAVITY;
					}

					gravModif = true;
				}
			}
			else
			{
				if (ImGui::InputFloat("##inputGrav", &gravRange.x, 1, 5, "%.2f"))
				{
					if (gravRange.x > MAX_GRAVITY)
					{
						gravRange.x = MAX_GRAVITY;
					}
					else if (gravRange.x < 0)
					{
						gravRange.x = 0;
					}

					gravModif = true;
				}
			}
			ImGui::SameLine(0.0f, 5.0f);
			ImGui::Text("Random");
			ImGui::SameLine(0.0f, 5.0f);
			if (ImGui::Checkbox("##randomGrav", &randomGrav))
			{
				emitter->SetRandomGravity(randomGrav);
			}

			ImGui::TableNextColumn();
			ImGui::Text("Color");
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(236.5f);

			if (ImGui::ColorEdit4("##color", &color[0]))
			{
				emitter->SetColor(color);
			}

			if (lifespanModif)
			{
				emitter->SetLifespanRange(lifespanRange);
			}
			if (speedModif)
			{
				emitter->SetSpeedRange(speedRange);
			}
			if (sizeModif)
			{
				emitter->SetSizeRange(sizeRange);
			}
			if (rotModif)
			{
				emitter->SetRotationRange(rotRange);
			}
			if (gravModif)
			{
				emitter->SetGravityRange(gravRange);
			}
			ImGui::EndTable();
		}

		//TODO: Draw Emitter Modules
		for (ParticleModule* module : emitter->GetModules())
		{
			module->DrawImGui();
		}

		ImGui::Separator();
	}
	else
	{
		//TODO: Select a ParticleEmitter to assign to the EmitterInstance (it acts as a Resource for the instance)
	}

	ImGui::PopID();
}
