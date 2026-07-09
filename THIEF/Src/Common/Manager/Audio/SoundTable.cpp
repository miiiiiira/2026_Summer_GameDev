#include "SoundTable.h"

// サウンドID から サウンドパスを取得

// システム用
namespace SoundTable_System
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::SYS_BUTTON_1,	{"Data/Sound/System/SYS_BUTTON_1.wav",	SoundType::SE_2D, 200 } },
		{ SoundID::SYS_BUTTON_2,	{"Data/Sound/System/SYS_BUTTON_2.wav",	SoundType::SE_2D, 200 } },
		{ SoundID::SYS_SELECT_ON,	{"Data/Sound/System/SYS_SELECT_ON.ogg",	SoundType::SE_2D, 200 } },
		{ SoundID::SYS_SELECT_CLICK,{"Data/Sound/System/system.mp3",		SoundType::SE_2D, 255 } },
		{ SoundID::SYS_ENTER,		{"Data/Sound/System/system.mp3",		SoundType::SE_2D, 255 } },
		{ SoundID::SYS_CANCEL,		{"Data/Sound/System/system.mp3",		SoundType::SE_2D, 255 } },
		{ SoundID::SYS_PAUSE_ON,	{"Data/Sound/System/SYS_PAUSE_ON.wav",	SoundType::SE_2D, 200 } },
		{ SoundID::SYS_PAUSE_OFF,	{"Data/Sound/System/SYS_PAUSE_OFF.wav",	SoundType::SE_2D, 200 } },
	};
}

// タイトル画面用
namespace SoundTable_Title
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::BGM_TITLE, {"Data/Sound/Title/BGM_TITLE.wav", SoundType::BGM, 255} },
	};
}

// メインメニュー画面用
namespace SoundTable_MainMenu
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::BGM_MAINMENU, {"Data/Sound/MainMenu/BGM_MAINMENU.wav", SoundType::BGM, 127} },
	};
}

// ゲーム画面用
namespace SoundTable_Game
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::BGM_GAME_1, {"Data/Sound/Game/BGM/BGM_GAME_1.wav", SoundType::BGM, 150} },
		{ SoundID::BGM_GAME_2, {"Data/Sound/Game/BGM/BGM_GAME_2.wav", SoundType::BGM, 150} },
		{ SoundID::BGM_GAME_3, {"Data/Sound/Game/BGM/BGM_GAME_3.wav", SoundType::BGM, 150} },

		{ SoundID::AMBIENT_SOUND_1, {"Data/Sound/Game/Ambient/AMBIENT_SOUND_1.wav", SoundType::SE_2D, 255} },
		{ SoundID::AMBIENT_SOUND_2, {"Data/Sound/Game/Ambient/AMBIENT_SOUND_2.wav", SoundType::SE_2D, 255} },
		{ SoundID::AMBIENT_SOUND_3, {"Data/Sound/Game/Ambient/AMBIENT_SOUND_3.wav", SoundType::SE_2D, 255} },
		{ SoundID::AMBIENT_SOUND_4, {"Data/Sound/Game/Ambient/AMBIENT_SOUND_4.wav", SoundType::SE_2D, 255} },

		{ SoundID::SE_JUMP,			{"Data/Sound/Game/Player/SE_JUMP.mp3",			SoundType::SE_2D, 200} },
		{ SoundID::SE_SLIDING,		{"Data/Sound/Game/Player/SE_SLIDING.wav",		SoundType::SE_2D, 255} },
		{ SoundID::SE_CROUCH,		{"Data/Sound/Game/Player/SE_CROUCH.wav",		SoundType::SE_2D, 255} },
		{ SoundID::SE_LANTERN_ON,	{"Data/Sound/Game/Player/SE_LANTERN_ON.wav",	SoundType::SE_2D, 127} },
		{ SoundID::SE_LANTERN_OFF,	{"Data/Sound/Game/Player/SE_LANTERN_OFF.wav",	SoundType::SE_2D, 200} },
		{ SoundID::SE_GRAB,			{"Data/Sound/Game/Player/SE_GRAB.wav",			SoundType::SE_2D, 255} },
		{ SoundID::SE_DAMAGE,		{"Data/Sound/Game/Player/SE_DAMAGE.mp3",		SoundType::SE_2D, 200} },
		{ SoundID::SE_MOVE,			{"Data/Sound/Game/Player/SE_MOVE.wav",			SoundType::SE_2D, 100} },
		{ SoundID::SE_DASH,			{"Data/Sound/Game/Player/SE_DASH.wav",			SoundType::SE_2D, 255} },

		{ SoundID::SE_DELIVERY_ITEM_ON,	{"Data/Sound/Game/Delivery/SE_DELIVERY_ITEM_ON.mp3", SoundType::SE_2D, 180} },
		{ SoundID::SE_DELIVERY_BUTTON_SUC, {"Data/Sound/Game/Delivery/SE_DELIVERY_BUTTON_SUC.mp3", SoundType::SE_2D, 200} },
		{ SoundID::SE_DELIVERY_BUTTON_FAI, {"Data/Sound/Game/Delivery/SE_DELIVERY_BUTTON_FAI.mp3", SoundType::SE_2D, 200} },
		{ SoundID::SE_CART_ON_ITEM,	{"Data/Sound/Game/Delivery/SE_DELIVERY_ITEM_ON.mp3", SoundType::SE_3D, 150} },

		{ SoundID::SE_ITEM_DAMAGE_1, {"Data/Sound/Game/Item/SE_ITEM_DAMAGE_1.mp3", SoundType::SE_3D, 100} },
		{ SoundID::SE_ITEM_DAMAGE_2, {"Data/Sound/Game/Item/SE_ITEM_DAMAGE_2.mp3", SoundType::SE_3D, 100} },
		{ SoundID::SE_ITEM_DAMAGE_3, {"Data/Sound/Game/Item/SE_ITEM_DAMAGE_3.mp3", SoundType::SE_3D, 100} },
		{ SoundID::SE_ITEM_DAMAGE_4, {"Data/Sound/Game/Item/SE_ITEM_DAMAGE_4.mp3", SoundType::SE_3D, 100} },

		{ SoundID::SE_ITEM_BREAK_1, {"Data/Sound/Game/Item/SE_ITEM_BREAK_1.wav", SoundType::SE_3D, 200} },
		{ SoundID::SE_ITEM_BREAK_2, {"Data/Sound/Game/Item/SE_ITEM_BREAK_2.wav", SoundType::SE_3D, 200} },
		{ SoundID::SE_ITEM_BREAK_3, {"Data/Sound/Game/Item/SE_ITEM_BREAK_3.mp3", SoundType::SE_3D, 200} },
		{ SoundID::SE_ITEM_BREAK_4, {"Data/Sound/Game/Item/SE_ITEM_BREAK_4.mp3", SoundType::SE_3D, 200} },
		{ SoundID::SE_ITEM_BREAK_5, {"Data/Sound/Game/Item/SE_ITEM_BREAK_5.mp3", SoundType::SE_3D, 127} },
		{ SoundID::SE_ITEM_BREAK_6, {"Data/Sound/Game/Item/SE_ITEM_BREAK_6.mp3", SoundType::SE_3D, 200} },
		{ SoundID::SE_ITEM_BREAK_7, {"Data/Sound/Game/Item/SE_ITEM_BREAK_7.mp3", SoundType::SE_3D, 127} },

		{ SoundID::SE_ENEMY_SPAWN_1, {"Data/Sound/Game/Enemy/SE_ENEMY_SPAWN_1.wav", SoundType::SE_3D, 255} },
		{ SoundID::SE_ENEMY_SPAWN_2, {"Data/Sound/Game/Enemy/SE_ENEMY_SPAWN_2.wav", SoundType::SE_3D, 255} },
		{ SoundID::SE_ENEMY_SPAWN_3, {"Data/Sound/Game/Enemy/SE_ENEMY_SPAWN_3.wav", SoundType::SE_3D, 255} },
		{ SoundID::SE_ENEMY_SPAWN_4, {"Data/Sound/Game/Enemy/SE_ENEMY_SPAWN_4.wav", SoundType::SE_3D, 255} },

		{ SoundID::SE_ENEMY_GIGGLE, {"Data/Sound/Game/Enemy/Giggle.mp3", SoundType::SE_2D, 100} },
		{ SoundID::SE_ENEMY_YETI, {"Data/Sound/Game/Enemy/roar.mp3", SoundType::SE_2D, 150} },
		{ SoundID::SE_ENEMY_MUSHNUB, {"Data/Sound/Game/Enemy/duck.mp3", SoundType::SE_2D, 150} },
		{ SoundID::SE_ENEMY_SKELETON, {"Data/Sound/Game/Enemy/Destruction.mp3", SoundType::SE_2D, 100} },
		{ SoundID::SE_ENEMY_STATUE, {"Data/Sound/Game/Enemy/Dragging.mp3", SoundType::SE_2D, 150} },
	};
}

// メインメニュー画面用
namespace SoundTable_Shop
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::BGM_SHOP, {"Data/Sound/Game/Shop/BGM_SHOP.wav", SoundType::SE_2D, 80} },

		{ SoundID::SE_SHOP_BUY_HP_UP, {"Data/Sound/Game/Shop/SE_SHOP_BUY_HP_UP.mp3", SoundType::SE_2D, 200} },
		{ SoundID::SE_SHOP_BUY_STAMINA_UP, {"Data/Sound/Game/Shop/SE_SHOP_BUY_STAMINA_UP.mp3", SoundType::SE_2D, 200} },
		{ SoundID::SE_SHOP_BUY_DASH_UP, {"Data/Sound/Game/Shop/SE_SHOP_BUY_DASH_UP.mp3", SoundType::SE_2D, 200} },
		{ SoundID::SE_SHOP_BUY_JUMP_UP, {"Data/Sound/Game/Shop/SE_SHOP_BUY_JUMP_UP.mp3", SoundType::SE_2D, 200} },
		{ SoundID::SE_SHOP_BUY_RANGE_UP, {"Data/Sound/Game/Shop/SE_SHOP_BUY_RANGE_UP.mp3", SoundType::SE_2D, 200} },
		{ SoundID::SE_SHOP_BUY_HEAL_25, {"Data/Sound/Game/Shop/SE_SHOP_BUY_HEAL_25.mp3", SoundType::SE_2D, 200} },
		{ SoundID::SE_SHOP_BUY_HEAL_50, {"Data/Sound/Game/Shop/SE_SHOP_BUY_HEAL_50.mp3", SoundType::SE_2D, 150} },
		{ SoundID::SE_SHOP_NOT_MONEY, {"Data/Sound/Game/Shop/SE_SHOP_NOT_MONEY.mp3", SoundType::SE_2D, 200} },

	};
}
