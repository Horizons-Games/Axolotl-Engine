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
}
} // namespace NPC

namespace AMBIENT
{
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