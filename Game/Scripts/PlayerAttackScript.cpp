#include "PlayerAttackScript.h"

#include "Application.h"

#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Physics/Physics.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentScript.h"
#include "Animation/AnimationController.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/PlayerManagerScript.h"
#include "../Scripts/PlayerMoveScript.h"
#include "../Scripts/EntityDetection.h"
#include "../Scripts/JumpFinisherAttack.h"
#include "../Scripts/JumpFinisherArea.h"
#include "../Scripts/LightFinisherAttackScript.h"
#include "../Scripts/HeavyFinisherAttack.h"
#include "../Scripts/LightAttackBullet.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include "../MathGeoLib/Include/Geometry/Ray.h"

#include "debugdraw.h"

#include <set>

#include "AxoLog.h"
#include "Defines/AxoTodo.h"

REGISTERCLASS(PlayerAttackScript);

PlayerAttackScript::PlayerAttackScript() : Script(), 
	canAttack(true), isAttacking(false), attackCooldown(0.6f), attackCooldownCounter(0.0f), audioSource(nullptr),
	animation(nullptr), transform(nullptr), isMelee(true),
	playerManager(nullptr), attackComboPhase(AttackCombo::IDLE), enemyDetection(nullptr), jumpFinisherScript(nullptr),
	lightFinisherScript(nullptr), normalAttackDistance(0.0f), heavyFinisherAttack(nullptr), lightWeapon(nullptr),
	comboCountHeavy(10.0f), comboCountLight(30.0f), comboCountJump(20.0f), triggerNextAttackDuration(0.5f), 
	triggerNextAttackTimer(0.0f), isNextAttackTriggered(false), currentAttackAnimation(""),
	numAttackComboAnimation(0.0f), isHeavyFinisherReceivedAux(false)
{
	REGISTER_FIELD(comboCountHeavy, float);
	REGISTER_FIELD(comboCountLight, float);
	REGISTER_FIELD(comboCountJump, float);

	REGISTER_FIELD(attackSoft, float);
	REGISTER_FIELD(attackHeavy, float);
	REGISTER_FIELD(normalAttackDistance, float);

	REGISTER_FIELD(isAttacking, bool);
	REGISTER_FIELD(isMelee, bool);

	REGISTER_FIELD(enemyDetection, EntityDetection*);
	REGISTER_FIELD(heavyFinisherAttack, HeavyFinisherAttack*);
	REGISTER_FIELD(lightWeapon, GameObject*);

	REGISTER_FIELD(bulletPrefab, GameObject*);
	REGISTER_FIELD(bulletVelocity, float);

	REGISTER_FIELD(pistolGameObject, GameObject*);

	REGISTER_FIELD(triggerNextAttackDuration, float);
}

void PlayerAttackScript::Start()
{
	audioSource = owner->GetComponent<ComponentAudioSource>();
	transform = owner->GetComponent<ComponentTransform>();
	animation = owner->GetComponent<ComponentAnimation>();

	if (isMelee)
	{
		audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_OPEN);
		audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_HUM);
	}

	playerManager = owner->GetComponent<PlayerManagerScript>();
	comboSystem = owner->GetComponent<ComboManager>();

	jumpFinisherScript = owner->GetComponent<JumpFinisherAttack>();
	lightFinisherScript = owner->GetComponent<LightFinisherAttackScript>();

	loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	if (pistolGameObject) 
	{
		pistolGameObject->Disable();
	}

	triggerNextAttackTimer = triggerNextAttackDuration;
}

void PlayerAttackScript::Update(float deltaTime)
{

	if (!canAttack)
	{
		return;
	}

	// Mark the enemy that is going to be attacked
	UpdateEnemyDetection();

	// Check if the special was activated
	comboSystem->CheckSpecial(deltaTime);

	if (animation->GetController()->GetStateName() == "DashingEnd")
	{
		if (!isAttacking)
		{
			AttackAfterDash();
		}
	}
	else if ((lastAttack == AttackType::LIGHTNORMAL || lastAttack == AttackType::HEAVYNORMAL)
		&& !isNextAttackTriggered) //Reset attack combo animation
	{
		animation->SetParameter("IsLightAttacking", false);
	}

	if (!IsAttackAvailable())
	{
		if (jumpFinisherScript->IsActive())
		{
			UpdateJumpAttack();
		}
		else
		{
			ResetAttackAnimations(deltaTime);
		}
	}
	
	PerformCombos();
}

void PlayerAttackScript::AttackAfterDash()
{
	switch (lastAttack)
	{
	case AttackType::LIGHTNORMAL:
	case AttackType::HEAVYNORMAL:
	{
		LOG_VERBOSE("Normal Attack Soft After Dash");
		if (lastAttack == AttackType::LIGHTNORMAL)
		{
			LightNormalAttack();
			lastAttack = AttackType::LIGHTNORMAL;
			currentAttackAnimation = "LightAttack";
		}
		else
		{
			HeavyNormalAttack();
			lastAttack = AttackType::HEAVYNORMAL;
			currentAttackAnimation = "LightAttack"; //Change if new heavy animations are implemented
		}
		isAttacking = true;
		break;
	}

	default:
		break;
	}
}

void PlayerAttackScript::UpdateEnemyDetection()
{
	if (comboSystem->NextIsSpecialAttack())
	{
		enemyDetection->UpdateEnemyDetection();
	}
	else
	{
		enemyDetection->UpdateEnemyDetection(normalAttackDistance);
	}
}

void PlayerAttackScript::PerformCombos()
{
	//Check input
	if (!IsPerformingJumpAttack())
	{
		currentAttack = comboSystem->CheckAttackInput(!playerManager->IsGrounded());
	}

	if (!IsAttackAvailable()) //Stack next attack if attack input called when it's currently in an attack animation
	{
		if (!isNextAttackTriggered)
		{
			switch (currentAttack)
			{
			case AttackType::LIGHTNORMAL:
			case AttackType::HEAVYNORMAL:
				if (lastAttack == AttackType::LIGHTNORMAL || lastAttack == AttackType::HEAVYNORMAL)
				{
					lastAttack = currentAttack;
					isNextAttackTriggered = true;
					triggerNextAttackTimer = triggerNextAttackDuration;
					if (numAttackComboAnimation == 2.0f) //Move between three attack animations
					{
						numAttackComboAnimation = 0.0f;
					}
					else
					{
						numAttackComboAnimation += 1.0f;
					}
					animation->SetParameter("NumAttackCombo", numAttackComboAnimation);
					animation->SetParameter("IsLightAttacking", true);
				}
				else if (playerManager->GetPlayerState() == PlayerActions::DASHING)
				{
					lastAttack = currentAttack;
					if (numAttackComboAnimation == 2.0f) //Move between three attack animations
					{
						numAttackComboAnimation = 0.0f;
					}
					else
					{
						numAttackComboAnimation += 1.0f;
					}
					animation->SetParameter("NumAttackCombo", numAttackComboAnimation);
				}
				break;

			case AttackType::HEAVYFINISHER:
				triggerNextAttackTimer = triggerNextAttackDuration;
				currentAttackAnimation = animation->GetController()->GetStateName();
				numAttackComboAnimation = 0.0f;
				animation->SetParameter("NumAttackCombo", numAttackComboAnimation);
				animation->SetParameter("HeavyFinisherInit", true);
				isAttacking = false;
				break;
			case AttackType::JUMPFINISHER:
				break;
			case AttackType::JUMPNORMAL:
				break;
			case AttackType::LIGHTFINISHER:
				triggerNextAttackTimer = triggerNextAttackDuration;
				currentAttackAnimation = animation->GetController()->GetStateName();
				numAttackComboAnimation = 0.0f;
				animation->SetParameter("NumAttackCombo", numAttackComboAnimation);
				animation->SetParameter("IsLightAttacking", false);
				isAttacking = false;
				LOG_VERBOSE("ResettingLightAttackAnimation");
				break;
			}
		}
	}

	if (IsAttackAvailable() && playerManager->GetPlayerState() != PlayerActions::DASHING)
	{
		switch (currentAttack)
		{
			case AttackType::LIGHTNORMAL:
				if (!isNextAttackTriggered) //Calling only when is not currently attacking
				{
					LOG_VERBOSE("Normal Normal Attack Soft");
					numAttackComboAnimation = 0.0f;
					animation->SetParameter("NumAttackCombo", numAttackComboAnimation);
					LightNormalAttack();
					isNextAttackTriggered = false;
					lastAttack = currentAttack;
					currentAttackAnimation = "LightAttack";
				}
				break;
			case AttackType::HEAVYNORMAL:
				if (!isNextAttackTriggered) //Calling only when is not currently attacking
				{
					LOG_VERBOSE("Normal Normal Attack Soft");
					numAttackComboAnimation = 0.0f;
					animation->SetParameter("NumAttackCombo", numAttackComboAnimation);
					HeavyNormalAttack();
					isNextAttackTriggered = false;
					lastAttack = currentAttack;
					currentAttackAnimation = "LightAttack"; //Change if new heavy animations are implemented
				}
				break;

			case AttackType::JUMPNORMAL:
				LOG_VERBOSE("Normal Attack Jump");
				InitJumpAttack();
				lastAttack = currentAttack;
				currentAttackAnimation = "JumpAttack";
				break;

			case AttackType::LIGHTFINISHER:
				LOG_VERBOSE("Finisher Soft");
				LightFinisher();
				lastAttack = currentAttack;
				currentAttackAnimation = "LightAttackFinish";
				break;

			case AttackType::HEAVYFINISHER:
				LOG_VERBOSE("Finisher Heavy");
				HeavyFinisher();
				lastAttack = currentAttack;
				currentAttackAnimation = "HeavyAttackFinish";
				break;

			case AttackType::JUMPFINISHER:
				LOG_VERBOSE("Finisher Jump");
				InitJumpAttack();
				lastAttack = currentAttack;
				currentAttackAnimation = "JumpAttack";
				break;

			default:
				break;
		}
	}
}

void PlayerAttackScript::LightNormalAttack()
{
	//Activate visuals and audios
	animation->SetParameter("IsLightAttacking", true);

	//Check collisions and Apply Effects
	GameObject* enemyAttacked = enemyDetection->GetEnemySelected();

	if (isMelee)
	{
		audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_SWING);
		if (enemyAttacked != nullptr)
		{
			LOG_VERBOSE("Enemy hit with light attack");
			comboSystem->SuccessfulAttack(comboCountLight, AttackType::LIGHTNORMAL);
			DamageEnemy(enemyAttacked, attackSoft);
		}
		else
		{
			LOG_VERBOSE("Fail light attack");
		}
	}
	else
	{
		audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::CANNON_SHOT);
		if (enemyAttacked != nullptr)
		{
			comboSystem->SuccessfulAttack(comboCountLight, AttackType::LIGHTNORMAL);
		}
		ThrowBasicAttack(enemyAttacked, attackSoft);
	}
	isAttacking = true;
}

void PlayerAttackScript::HeavyNormalAttack()
{
	//Activate visuals and audios
	animation->SetParameter("IsLightAttacking", true); //Change if new heavy animations are implemented

	//Check collisions and Apply Effects
	GameObject* enemyAttacked = enemyDetection->GetEnemySelected();
	
	if (isMelee)
	{
		audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_SWING);
		if (enemyAttacked != nullptr)
		{
			LOG_VERBOSE("Enemy hit with heavy attack");
			comboSystem->SuccessfulAttack(comboCountHeavy, AttackType::HEAVYNORMAL);
			DamageEnemy(enemyAttacked, attackHeavy);
		}
		else
		{
			LOG_VERBOSE("Fail heavy attack");
		}
	}
	else
	{
		audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::CHARGED_SHOT);
		if (enemyAttacked != nullptr)
		{
			comboSystem->SuccessfulAttack(comboCountHeavy, AttackType::HEAVYNORMAL);
		}
		ThrowBasicAttack(enemyAttacked, attackHeavy);
	}
	
	isAttacking = true;
}

void PlayerAttackScript::ThrowBasicAttack(GameObject* enemyAttacked, float nDamage)
{
	// Create a new bullet
	GameObject* bullet = loadedScene->DuplicateGameObject(bulletPrefab->GetName(), bulletPrefab, owner);
	LightAttackBullet* ligthAttackBulletScript = bullet->GetComponent<LightAttackBullet>();

	ligthAttackBulletScript->SetBulletVelocity(bulletVelocity);
	ligthAttackBulletScript->SetEnemy(enemyDetection->GetEnemySelected());
	ligthAttackBulletScript->SetStunTime(0);
	ligthAttackBulletScript->SetDamage(nDamage);
}

void PlayerAttackScript::InitJumpAttack()
{
	animation->SetParameter("IsJumpAttacking", true);
	isAttacking = true;
	playerManager->GetMovementManager()->SetIsTriggeringStoredDash(false);
	
	playerManager->ParalyzePlayer(true);

	if (isMelee)
	{
		jumpFinisherScript->PerformGroundSmash(); // Bix jumping attack
	}
	else
	{
		jumpFinisherScript->ShootForceBullet(10.0f, 2.0f); // Allura jumping attack, placed it here for now
	}
}

void PlayerAttackScript::UpdateJumpAttack()
{
	bool landed = false;
	//if (isMelee) landing is player grounded if not then is the projectile detection for the moment I only put this

	if (isMelee)
	{
		landed = playerManager->IsGrounded();
	}
	else
	{
		AXO_TODO("Add Allura Checks")
		landed = true;
	}

	if (landed)
	{
		animation->SetParameter("IsJumpAttacking", false);
		if (currentAttack == AttackType::JUMPNORMAL)
		{
			EndJumpNormalAttack();
		}
		else
		{
			EndJumpFinisherAttack();
		}
		currentAttack = AttackType::NONE;
	}
}

void PlayerAttackScript::EndJumpNormalAttack()
{
	jumpFinisherScript->VisualLandingEffect();
	if (enemyDetection->AreAnyEnemiesInTheArea())
	{
		jumpFinisherScript->PushEnemies(10.0f, 2.0f, enemyDetection->GetEnemiesInTheArea());
		comboSystem->SuccessfulAttack(comboCountJump, currentAttack);
	}
}

void PlayerAttackScript::EndJumpFinisherAttack()
{
	jumpFinisherScript->VisualLandingEffect();
	if (enemyDetection->AreAnyEnemiesInTheArea())
	{
		jumpFinisherScript->PushEnemies(15.0f, 4.0f, enemyDetection->GetEnemiesInTheArea());
		comboSystem->SuccessfulAttack(-35.0f, currentAttack);
	}
}

void PlayerAttackScript::LightFinisher()
{
	animation->SetParameter("LightFinisherAttacking", true);

	if (pistolGameObject)
	{
		pistolGameObject->Enable();
	}

	isAttacking = true;

	if (!isMelee)
	{
		audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::ELECTRIC_SHOT);
	}
	lightFinisherScript->ThrowStunItem();

	comboSystem->SuccessfulAttack(-comboCountLight * 2, AttackType::LIGHTFINISHER);
}

void PlayerAttackScript::HeavyFinisher()
{
	if (lightWeapon)
	{
		lightWeapon->Disable();
	}
	GameObject* enemyAttacked = enemyDetection->GetEnemySelected();
	animation->SetParameter("HeavyFinisherEnd", false);
	animation->SetParameter("HeavyFinisherInit", true);
	isAttacking = true;
	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::CANNON_SHOT);
	if (enemyAttacked != nullptr)
	{
		heavyFinisherAttack->PerformHeavyFinisher(enemyAttacked->GetComponent<ComponentTransform>(), 
			GetOwner()->GetComponent<ComponentTransform>());
		comboSystem->SuccessfulAttack(-comboCountHeavy * 2, AttackType::HEAVYFINISHER);
	}
	else
	{
		heavyFinisherAttack->PerformEmptyHeavyFinisher(GetOwner()->GetComponent<ComponentTransform>());
		comboSystem->SuccessfulAttack(-comboCountHeavy * 2, AttackType::HEAVYFINISHER);
	}
}

//void PlayerAttackScript::JumpFinisher()
//{
//	animation->SetParameter("IsJumpAttacking", true);
//	isAttacking = true;
//
//	if(isMelee)
//	{
//		jumpFinisherScript->PerformGroundSmash(15.0f, 4.0f); // Bix jumping finisher
//	}
//	else
//	{
//		jumpFinisherScript->ShootForceBullet(15.0f, 4.0f); // Allura jumping finisher, placed it here for now
//	}
//
//	comboSystem->SuccessfulAttack(-comboCountJump * 2, AttackType::JUMPFINISHER);
//}

void PlayerAttackScript::ResetAttackAnimations(float deltaTime)
{
	switch (lastAttack)
	{
		case AttackType::LIGHTNORMAL:
		case AttackType::HEAVYNORMAL:
		{
			if (animation->GetController()->GetStateName() != currentAttackAnimation)
			{
				if (isNextAttackTriggered)
				{
					currentAttackAnimation = animation->GetController()->GetStateName();
					LOG_VERBOSE("Normal Attack Soft");
					if (lastAttack == AttackType::LIGHTNORMAL)
					{
						LightNormalAttack();
						lastAttack = AttackType::LIGHTNORMAL;
					}
					else
					{
						HeavyNormalAttack();
						lastAttack = AttackType::HEAVYNORMAL;
					}
					isAttacking = true;
					isNextAttackTriggered = false;
				}
				else //Called when next attack input is not called
				{
					triggerNextAttackTimer -= deltaTime;
					if (triggerNextAttackTimer <= 0.0f || playerManager->GetMovementManager()->IsTriggeringStoredDash()) //Wait to reset the offset time, to give the player the chance to
						//trigger the next attack even if the animation has finished (due to some animations are very short)
					{
						triggerNextAttackTimer = triggerNextAttackDuration;
						currentAttackAnimation = animation->GetController()->GetStateName();
						numAttackComboAnimation = 0.0f;
						animation->SetParameter("NumAttackCombo", numAttackComboAnimation);
						animation->SetParameter("IsLightAttacking", false);
						isAttacking = false;
						lastAttack = AttackType::NONE;
						LOG_VERBOSE("ResettingLightAttackAnimation");
					}
				}
			}
			break;
		}

		case AttackType::JUMPNORMAL:
		case AttackType::JUMPFINISHER:
		{
			std::string actualName = currentAttackAnimation;
			if (isMelee)
			{
				actualName = "JumpAttackRecovery";
			}
			if (animation->GetController()->GetStateName() != currentAttackAnimation)
			{
				playerManager->ParalyzePlayer(false);
				animation->SetParameter("IsJumpAttacking", false);
				isAttacking = false;
				currentAttack = AttackType::NONE;
				lastAttack = AttackType::NONE;
			}

			// There are some times in which the animations happen so quick and the first if is not entered,
			// so I added this as a safe mesure because, if not, the player would be prevented of attacking,
			// jumping and moving if the first if is not entered

			/*else if (animation->GetActualStateName() != "JumpAttack")
			{
				isAttacking = false;
			}*/
			break;
		}

		case AttackType::LIGHTFINISHER:	
			if (animation->GetController()->GetStateName() != currentAttackAnimation)
			{
				animation->SetParameter("LightFinisherAttacking", false);
				isAttacking = false;
				lastAttack = AttackType::NONE;
				if (pistolGameObject)
				{
					pistolGameObject->Disable();
				}
			}
			break;	

		case AttackType::HEAVYFINISHER:
			animation->SetParameter("HeavyFinisherInit", false);
			if (!heavyFinisherAttack->IsAttacking()) 
			{
				currentAttackAnimation = "HeavyAttackFinishEnd";
				animation->SetParameter("HeavyFinisherEnd", true);
				if (lightWeapon)
				{
					lightWeapon->Enable();
				}
				if (isHeavyFinisherReceivedAux)
				{
					animation->SetParameter("HeavyFinisherEnd", false);
					if (animation->GetController()->GetStateName() != currentAttackAnimation)
					{
						currentAttackAnimation = "";
						isAttacking = false;
						lastAttack = AttackType::NONE;
						isHeavyFinisherReceivedAux = false;
						break;
					}
				}
				isHeavyFinisherReceivedAux = true;
			}
			break;

		default:
			break;
	}
}

void PlayerAttackScript::DamageEnemy(GameObject* enemyAttacked, float damageAttack) 
{
	if (enemyAttacked != nullptr)
	{
		HealthSystem* healthScript = enemyAttacked->GetComponent<HealthSystem>();
		float attack = isDeathTouched ? healthScript->GetMaxHealth() : damageAttack;
		healthScript->TakeDamage(attack);
		audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_CLASH);
	}
}

bool PlayerAttackScript::IsAttackAvailable() const
{
	return !isAttacking;
}

bool PlayerAttackScript::IsMeleeAvailable() const
{
	return isMelee;
}

bool PlayerAttackScript::IsPerformingJumpAttack() const
{
	return (currentAttack == AttackType::JUMPFINISHER || currentAttack == AttackType::JUMPNORMAL);
}

bool PlayerAttackScript::CanAttack() const
{
	return canAttack;
}

void PlayerAttackScript::SetCanAttack(bool canAttack)
{

	this->canAttack = canAttack;
}

bool PlayerAttackScript::IsDeathTouched() const
{
	return isDeathTouched;
}

void PlayerAttackScript::SetIsDeathTouched(bool isDeathTouched)
{
	this->isDeathTouched = isDeathTouched;
}

AttackType PlayerAttackScript::GetCurrentAttackType() const
{
	return lastAttack;
}

bool PlayerAttackScript::IsInAttackAnimation() const
{
	std::string animString = animation->GetController()->GetStateName();
	return animString == "LightAttack" || animString == "SecondAttack" || animString == "HeavyAttack";
}

GameObject* PlayerAttackScript::GetEnemyDetected() const
{
	return enemyDetection->GetEnemySelected();
}