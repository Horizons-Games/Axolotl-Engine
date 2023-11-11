#pragma once

namespace AUDIO
{
namespace SFX
{
namespace NPC
{
static const wchar_t* FOOTSTEPS = L"EnemyFootsteps";
static const wchar_t* FOOTSTEPS_STOP = L"EnemyFootstepsStop";
static const wchar_t* DEATH = L"EnemyDeath";
static const wchar_t* ALERT = L"EnemyAlert";
static const wchar_t* SHOT_IMPACT = L"EnemyShotImpact";
namespace DRON
{
static const wchar_t* SHOT = L"sfx_dron_shot_01";
static const wchar_t* SHOT_CHARGE = L"DronPreShot";
static const wchar_t* TIMER = L"sfx_dron_death_timer";
static const wchar_t* STOP_TIMER = L"sfx_dron_death_timer_stop";
static const wchar_t* EXPLOSION = L"sfx_drone_death_explosion";
static const wchar_t* PATROL = L"sfx_dron_patrol";
static const wchar_t* SEEK = L"sfx_dron_alert";
static const wchar_t* STOP_BEHAVIOURS = L"sfx_dron_behaviours_stop";
} // namespace DRON
namespace VENOMITE
{
static const wchar_t* SHOT = L"VenomiteShot";
static const wchar_t* SHOT_CHARGE = L"VenomitePreShot";
} // namespace VENOMITE
namespace MINIBOSS2
{
static const wchar_t* AIM = L"sfx_boostOfEnergy_aiming";
static const wchar_t* AIM_STOP = L"sfx_boostOfEnergy_aiming_stop";
static const wchar_t* PRESHOT = L"sfx_boostOfEnergy_preShot";
static const wchar_t* SHOT = L"sfx_boostOfEnergy_shot";
static const wchar_t* SHOT_STOP = L"sfx_boostOfEnergy_shot_stop";
} // namespace MINIBOSS2
namespace FINALBOSS
{
static const wchar_t* ENERGYSHIELD = L"FinalBoss_EnergyShield";
static const wchar_t* ENERGYSHIELD_STOP = L"FinalBoss_EnergyShieldStop";
static const wchar_t* CHARGE_ATTACK = L"FinalBoss_ChargeAttack";
static const wchar_t* CHARGE_WALL_HIT = L"FinalBoss_WallHit";
static const wchar_t* CHARGE_WALL_STUNNED = L"FinalBoss_Stunned";
static const wchar_t* CHARGE_WALL_STUNNED_STOP = L"FinalBoss_StunnedStop";
static const wchar_t* CHARGE_ROCKS = L"FinalBoss_RocksFalling";
static const wchar_t* CHARGE_ROCKS_IMPACT = L"FinalBoss_RocksImpact";
static const wchar_t* HAMMER_HIT = L"FinalBoss_HammerAttack";
static const wchar_t* HAMMER_FIRE_ON = L"FinalBoss_HammerOnFire";
static const wchar_t* ROCKETS_LAUNCH = L"FinalBoss_RocketLaunch";
static const wchar_t* ROCKET_IMPACT = L"FinalBoss_RocketImpact";
static const wchar_t* ROCKET_FALLING = L"FinalBoss_RocketFalling";
static const wchar_t* SUMMON_ENEMIES = L"FinalBoss_SummonEnemies";
static const wchar_t* JUMP = L"FinalBoss_Jump";
static const wchar_t* JUMP_LANDING = L"FinalBoss_Land";
static const wchar_t* RECEIVE_DAMAGE = L"FinalBoss_Damage";
static const wchar_t* DEATH = L"FinalBoss_Death";
} // namespace FINALBOSS
namespace CANTINA
{
static const wchar_t* DRUNKNPC_RANDOM_SOUND = L"sfx_drunkNPC_sound_random";
static const wchar_t* CROWDNPC_LAUGHING = L"sfx_crowdNPC_laughing";
}
} // namespace NPC

namespace AMBIENT
{
namespace SEWERS
{
static const wchar_t* BIGDOOR_OPEN = L"sfx_bigDoor_open";
static const wchar_t* BIGDOOR_CLOSE = L"sfx_bigDoor_close";
static const wchar_t* TURBINE = L"sfx_turbine";
static const wchar_t* WATER_FALLING = L"sfx_water_falling";
static const wchar_t* BUBBLES = L"sfx_water_bubbles";
static const wchar_t* AMBIENT = L"sfx_sewers_ambient";
static const wchar_t* AMBIENT_STOP = L"sfx_sewers_ambient_stop";
} // namespace SEWERS
namespace CANTINA
{
static const wchar_t* CROWD_TALKING_01 = L"sfx_ambient_crowd_talking_01";
static const wchar_t* CANTINA_MUSIC = L"cantina_music";
static const wchar_t* STOP_AMBIENT_CANTINA = L"ambient_cantina_stop";
} // namespace CANTINA
namespace SPACESTATION
{
static const wchar_t* GENERATOR = L"PowerGenerator";
static const wchar_t* SMALL_DOOR_OPEN = L"SmallDoorOpen";
static const wchar_t* SMALL_DOOR_CLOSE = L"SmallDoorClose";
static const wchar_t* AMBIENT = L"SpaceStationAmbient";
static const wchar_t* AMBIENT_STOP = L"SpaceStationAmbientStop";
static const wchar_t* HACKING = L"HackingMachine";
static const wchar_t* ELEVATOR_START = L"ElevatorStart";
static const wchar_t* ELEVATOR_END = L"ElevatorEnd";
} // namespace SPACESTATION
namespace SPACESHIPESCAPE
{
static const wchar_t* FIRE = L"FireThrowerBurst";
static const wchar_t* FIRE_STOP = L"FireThrowerBurstStop";
static const wchar_t* TURBINE = L"BigTurbine";
static const wchar_t* WIND = L"WindAmbient";
} // namespace SPACESHIPESCAPE
} // namespace AMBIENT

namespace PLAYER
{
namespace LOCOMOTION
{
static const wchar_t* FOOTSTEPS_WALK_BIX_METAL = L"sfx_footsteps_walk_Bix_metal";
static const wchar_t* FOOTSTEPS_WALK_ALLURA_METAL = L"sfx_footsteps_walk_Allura_metal";
static const wchar_t* FOOTSTEPS_WALK_BIX_WATER = L"sfx_footsteps_walk_Bix_water";
static const wchar_t* FOOTSTEPS_WALK_ALLURA_WATER = L"sfx_footsteps_walk_Allura_water";
static const wchar_t* FOOTSTEPS_WALK_STOP = L"sfx_footsteps_walk_stop";
static const wchar_t* FALLING_METAL = L"sfx_falling_metal";
static const wchar_t* FALLING_WATER = L"sfx_falling_water";
static const wchar_t* ALLURA_JUMP = L"Allura_Jump";
static const wchar_t* BIX_JUMP = L"Bix_Jump";
static const wchar_t* ALLURA_DOUBLE_JUMP = L"Allura_DoubleJump";
static const wchar_t* BIX_DOUBLE_JUMP = L"Bix_DoubleJump";
static const wchar_t* DASH = L"sfx_dash_01";
static const wchar_t* ROLL = L"sfx_roll_01";
} // namespace LOCOMOTION
namespace WEAPON
{
static const wchar_t* LIGHTSABER_CLASH = L"Bix_LightsaberImpact";
static const wchar_t* LIGHTSABER_SWING = L"Bix_LightsaberAttack";
static const wchar_t* LIGHTSABER_DOWN_ATTACK = L"Bix_DownAttackAction";
static const wchar_t* LIGHTSABER_DOWN_ATTACK_IMPACT = L"Bix_DownAttackImpact";
static const wchar_t* LIGHTSABER_THROW = L"Bix_HeavyAttackFinisher";
static const wchar_t* LIGHTSABER_THROW_STOP = L"Bix_HeavyAttackFinisherStop";
static const wchar_t* ROCKS_THROW = L"Bix_LightAttackFinisher";
static const wchar_t* ROCKS_THROW_IMPACT = L"Bix_LightAttackFinisherImpact";
static const wchar_t* SHOT = L"Allura_WeaponShot";
static const wchar_t* SHOT_IMPACT = L"Allura_ShotImpact";
static const wchar_t* CHARGED_SHOT = L"Allura_LightAttackFinisher";
static const wchar_t* CHARGED_SHOT_IMPACT = L"Allura_LightAttackFinisherImpact";
static const wchar_t* ELECTRIC_SHOT = L"Allura_HeavyAttackFinisher";
static const wchar_t* ELECTRIC_SHOT_IMPACT = L"Allura_HeavyAttackFinisherImpact";
static const wchar_t* ALLURA_DOWN_ATTACK = L"Allura_DownAttackAction";
static const wchar_t* GRANADE_EXPLOSION = L"Allura_DownAttackImpact";
static const wchar_t* RECEIVEDAMAGE_BIX = L"Bix_ReceiveDamage";
static const wchar_t* RECEIVEDAMAGE_ALLURA = L"Allura_ReceiveDamage";
} // namespace WEAPON
namespace ABILITIES
{
static const wchar_t* FORCE_USE = L"sfx_force";
static const wchar_t* FORCE_STOP = L"sfx_force_stop";
} // namespace ABILITIES
} // namespace PLAYER

namespace UI
{
namespace MENU
{
static const wchar_t* BUTTON_BACK = L"sfx_button_back_01";
static const wchar_t* BUTTON_PRESS = L"sfx_button_press_01";
static const wchar_t* BUTTON_HOVER = L"sfx_button_hover_01";
} // namespace MENU
namespace HACKING
{
static const wchar_t* CORRECT = L"HackingCorrect";
static const wchar_t* WRONG = L"HackingWrong";
static const wchar_t* FINISHED = L"HackingFinished";
} // namespace HACKING
} // namespace UI

} // namespace SFX

namespace MUSIC
{
static const wchar_t* PLAY_MUSIC = L"Music";
static const wchar_t* PLAY_MUSIC_PART02 = L"Music_Part02";
static const wchar_t* STOP_MUSIC = L"StopMusic";

namespace SWITCH
{
namespace GROUP
{
static const wchar_t* GAMEPLAY = L"Gameplay";
}
namespace ID
{
namespace GAMEPLAY
{
static const wchar_t* EXPLORATION = L"Exploration";
static const wchar_t* COMBAT = L"Combat";
static const wchar_t* MAIN_MENU = L"MainMenu";
} // namespace GAMEPLAY
} // namespace ID
} // namespace SWITCH
} // namespace MUSIC

namespace STATES
{
namespace GROUP
{
static const wchar_t* LIFE = L"PlayerLife";
static const wchar_t* ZONE = L"Zone";
} // namespace GROUP

namespace ID
{
namespace PLAYERLIFE
{
static const wchar_t* ALIVE = L"Alive";
static const wchar_t* DEFEATED = L"Defeated";
static const wchar_t* NONE = L"None";
} // namespace PLAYERLIFE
namespace ZONE
{
static const wchar_t* SPACESTATION = L"SpaceStation";
static const wchar_t* SPACESHIP = L"Spaceship";
static const wchar_t* BOSS = L"Boss";
static const wchar_t* BOSS_INTRO = L"BossIntro";
static const wchar_t* HANGAR = L"Hangar";
static const wchar_t* SEWERS = L"Sewers";
static const wchar_t* CANTINA = L"Cantina";
static const wchar_t* NONE = L"None";
} // namespace ZONE
} // namespace ID
} // namespace STATES

namespace CONTROLLERS
{
static const wchar_t* MASTERVOLUME = L"MasterVolume";
static const wchar_t* MUSICVOLUME = L"MusicVolume";
static const wchar_t* SFXVOLUME = L"SFXVolume";
static const wchar_t* LOWPASSFILTER = L"LowPassFilterOnAction";
} // namespace CONTROLLERS
}; // namespace AUDIO