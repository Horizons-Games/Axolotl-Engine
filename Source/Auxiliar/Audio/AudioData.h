#pragma once

namespace AUDIO
{
namespace SFX
{
namespace NPC
{
namespace DRON
{
static const wchar_t* SHOT_01 = L"sfx_dron_shot_01";
static const wchar_t* SHOT_IMPACT_01 = L"sfx_dron_shot_impact_01";
static const wchar_t* PATROL = L"sfx_dron_patrol";
static const wchar_t* ALERT = L"sfx_dron_alert";
static const wchar_t* STOP_BEHAVIOURS = L"sfx_dron_behaviours_stop";
} // namespace DRON
namespace CANTINA
{
static const wchar_t* DRUNKNPC_RANDOM_SOUND = L"sfx_drunkNPC_sound_random";
}
} // namespace NPC

namespace AMBIENT
{
namespace SEWERS
{
static const wchar_t* BIGDOOR_OPEN = L"sfx_bigDoor_open";
static const wchar_t* BIGDOOR_CLOSE = L"sfx_bigDoor_close";
static const wchar_t* AMBIENT = L"sewers_ambient";
static const wchar_t* STOP_AMBIENT = L"sewers_ambient_stop";
} // namespace SEWERS
namespace CANTINA
{
static const wchar_t* CROWD_TALKING_01 = L"sfx_ambient_crowd_talking_01";
static const wchar_t* CANTINA_MUSIC = L"cantina_music";
static const wchar_t* STOP_AMBIENT_CANTINA = L"ambient_cantina_stop";
} // namespace CANTINA
} // namespace AMBIENT

namespace PLAYER
{
namespace LOCOMOTION
{
static const wchar_t* FOOTSTEPS_WALK = L"sfx_footsteps_walk_01";
static const wchar_t* FOOTSTEPS_WALK_STOP = L"sfx_footsteps_walk_stop";
static const wchar_t* FOOTSTEPS_RUN = L"sfx_footsteps_run_01";
static const wchar_t* FOOTSTEPS_RUN_STOP = L"sfx_footsteps_run_stop";
static const wchar_t* FOOTSTEPS_CROUCH = L"sfx_footsteps_crouch_01";
static const wchar_t* FOOTSTEPS_CROUCH_STOP = L"sfx_footsteps_crouch_stop";
static const wchar_t* JUMP = L"sfx_jump_01";
static const wchar_t* DOUBLE_JUMP = L"sfx_doubleJump_01";
static const wchar_t* DASH = L"sfx_dash_01";
} // namespace LOCOMOTION
namespace WEAPON
{
static const wchar_t* LIGHTSABER_CLASH = L"sfx_lightsaber_clash";
static const wchar_t* LIGHTSABER_OPEN = L"sfx_lightsaber_open_01";
static const wchar_t* LIGHTSABER_SWING = L"sfx_lightsaber_swing";
static const wchar_t* LIGHTSABER_CLOSE = L"sfx_lightsaber_close_01";
static const wchar_t* LIGHTSABER_HUM = L"sfx_lightsaber_hum_01";
static const wchar_t* LIGHTSABER_HUM_STOP = L"sfx_lightsaber_hum_stop";
} // namespace WEAPON
namespace ABILITIES
{
static const wchar_t* FORCE_USE = L"sfx_force";
static const wchar_t* FORCE_STOP = L"sfx_force_stop";
} // namespace ABILITIES
} // namespace PLAYER

namespace UI
{
static const wchar_t* BUTTON_BACK = L"sfx_button_back_01";
static const wchar_t* BUTTON_PRESS = L"sfx_button_press_01";
static const wchar_t* BUTTON_HOVER = L"sfx_button_hover_01";
} // namespace UI

} // namespace SFX

namespace MUSIC
{
static const wchar_t* PLAY_MUSIC = L"Music";
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
static const wchar_t* HANGAR = L"Hangar";
static const wchar_t* SEWERS = L"Sewers";
static const wchar_t* CANTINA = L"Cantina";
static const wchar_t* NONE = L"None";
} // namespace ZONE
} // namespace ID
} // namespace STATES
}; // namespace AUDIO