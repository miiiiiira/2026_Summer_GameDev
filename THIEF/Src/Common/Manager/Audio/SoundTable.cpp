#include "SoundTable.h"

// サウンドID から サウンドパスを取得

// システム用
namespace SoundTable_System
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::SYS_BUTTON_1, { "Data/Sound/System/SYS_BUTTON_1.wav", 200 } },
		{ SoundID::SYS_BUTTON_2, {"Data/Sound/System/SYS_BUTTON_2.wav", 200 } },
		{ SoundID::SYS_SELECT_ON, {"Data/Sound/System/SYS_SELECT_ON.ogg", 200 } },
		{ SoundID::SYS_SELECT_CLICK, {"Data/Sound/System/system.mp3", 255 } },
		{ SoundID::SYS_ENTER, {"Data/Sound/System/system.mp3", 255 } },
		{ SoundID::SYS_CANCEL, {"Data/Sound/System/system.mp3", 255 } },
		{ SoundID::SYS_PAUSE_ON, {"Data/Sound/System/SYS_PAUSE_ON.wav", 200 } },
		{ SoundID::SYS_PAUSE_OFF, {"Data/Sound/System/SYS_PAUSE_OFF.wav", 200 } },
	};
}

// タイトル画面用
namespace SoundTable_Title
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::BGM_TITLE, {"Data/Sound/Title/BGM_TITLE.wav",255} },
	};
}

// メインメニュー画面用
namespace SoundTable_MainMenu
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::BGM_MAINMENU, {"Data/Sound/MainMenu/BGM_MAINMENU.wav", 127} },
	};
}

// ゲーム画面用
namespace SoundTable_Game
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::BGM_GAME_1, {"Data/Sound/Game/BGM/BGM_GAME_1.wav", 150} },
		{ SoundID::BGM_GAME_2, {"Data/Sound/Game/BGM/BGM_GAME_2.wav", 150} },
		{ SoundID::BGM_GAME_3, {"Data/Sound/Game/BGM/BGM_GAME_3.wav", 150} },

		{ SoundID::AMBIENT_SOUND_1, {"Data/Sound/Game/Ambient/AMBIENT_SOUND_1.wav", 255} },
		{ SoundID::AMBIENT_SOUND_2, {"Data/Sound/Game/Ambient/AMBIENT_SOUND_2.wav", 255} },
		{ SoundID::AMBIENT_SOUND_3, {"Data/Sound/Game/Ambient/AMBIENT_SOUND_3.wav", 255} },
		{ SoundID::AMBIENT_SOUND_4, {"Data/Sound/Game/Ambient/AMBIENT_SOUND_4.wav", 255} },

		{ SoundID::SE_JUMP, {"Data/Sound/Game/Player/SE_JUMP.mp3", 200} },
		{ SoundID::SE_SLIDING, {"Data/Sound/Game/Player/SE_SLIDING.wav", 255} },
		{ SoundID::SE_CROUCH, {"Data/Sound/Game/Player/SE_CROUCH.wav", 255} },
		{ SoundID::SE_LANTERN_ON, {"Data/Sound/Game/Player/SE_LANTERN_ON.wav", 127} },
		{ SoundID::SE_LANTERN_OFF, {"Data/Sound/Game/Player/SE_LANTERN_OFF.wav", 200} },
		{ SoundID::SE_GRAB, {"Data/Sound/Game/Player/SE_GRAB.wav", 255} },
		{ SoundID::SE_DAMAGE, {"Data/Sound/Game/Player/SE_DAMAGE.wav", 255} },
		{ SoundID::SE_MOVE, {"Data/Sound/Game/Player/SE_MOVE.wav", 100} },
		{ SoundID::SE_DASH, {"Data/Sound/Game/Player/SE_DASH.wav", 255} },

		{ SoundID::SE_DELIVERY_ITEM_ON, {"Data/Sound/Game/Delivery/SE_DELIVERY_ITEM_ON.mp3", 180} },
		{ SoundID::SE_DELIVERY_BUTTON_SUC, {"Data/Sound/Game/Delivery/SE_DELIVERY_BUTTON_SUC.mp3", 200} },
		{ SoundID::SE_DELIVERY_BUTTON_FAI, {"Data/Sound/Game/Delivery/SE_DELIVERY_BUTTON_FAI.mp3", 200} },

		{ SoundID::SE_ITEM_DAMAGE_1, {"Data/Sound/Game/Item/SE_ITEM_DAMAGE_1.wav", 255} },
		{ SoundID::SE_ITEM_DAMAGE_2, {"Data/Sound/Game/Item/SE_ITEM_DAMAGE_2.wav", 255} },
		{ SoundID::SE_ITEM_DAMAGE_3, {"Data/Sound/Game/Item/SE_ITEM_DAMAGE_3.wav", 255} },
		{ SoundID::SE_ITEM_DAMAGE_4, {"Data/Sound/Game/Item/SE_ITEM_DAMAGE_4.wav", 255} },

		{ SoundID::SE_ITEM_BREAK_1, {"Data/Sound/Game/Item/SE_ITEM_BREAK_1.wav", 200} },
		{ SoundID::SE_ITEM_BREAK_2, {"Data/Sound/Game/Item/SE_ITEM_BREAK_2.wav", 200} },
		{ SoundID::SE_ITEM_BREAK_3, {"Data/Sound/Game/Item/SE_ITEM_BREAK_3.wav", 200} },
		{ SoundID::SE_ITEM_BREAK_4, {"Data/Sound/Game/Item/SE_ITEM_BREAK_4.wav", 200} },

		{ SoundID::SE_ENEMY_SPAWN_1, {"Data/Sound/Game/Enemy/SE_ENEMY_SPAWN_1.wav", 255} },
		{ SoundID::SE_ENEMY_SPAWN_2, {"Data/Sound/Game/Enemy/SE_ENEMY_SPAWN_2.wav", 255} },
		{ SoundID::SE_ENEMY_SPAWN_3, {"Data/Sound/Game/Enemy/SE_ENEMY_SPAWN_3.wav", 255} },
		{ SoundID::SE_ENEMY_SPAWN_4, {"Data/Sound/Game/Enemy/SE_ENEMY_SPAWN_4.wav", 255} },
	};
}

// メインメニュー画面用
namespace SoundTable_Shop
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::BGM_SHOP, {"Data/Sound/Game/Shop/BGM_SHOP.wav", 80} },

		{ SoundID::SE_SHOP_BUY_HP_UP, {"Data/Sound/Game/Shop/SE_SHOP_BUY_HP_UP.mp3", 200} },
		{ SoundID::SE_SHOP_BUY_STAMINA_UP, {"Data/Sound/Game/Shop/SE_SHOP_BUY_STAMINA_UP.mp3", 200} },
		{ SoundID::SE_SHOP_BUY_DASH_UP, {"Data/Sound/Game/Shop/SE_SHOP_BUY_DASH_UP.mp3", 200} },
		{ SoundID::SE_SHOP_BUY_JUMP_UP, {"Data/Sound/Game/Shop/SE_SHOP_BUY_JUMP_UP.mp3", 200} },
		{ SoundID::SE_SHOP_BUY_RANGE_UP, {"Data/Sound/Game/Shop/SE_SHOP_BUY_RANGE_UP.mp3", 200} },
		{ SoundID::SE_SHOP_BUY_HEAL_25, {"Data/Sound/Game/Shop/SE_SHOP_BUY_HEAL_25.mp3", 200} },
		{ SoundID::SE_SHOP_BUY_HEAL_50, {"Data/Sound/Game/Shop/SE_SHOP_BUY_HEAL_50.mp3", 150} },
		{ SoundID::SE_SHOP_NOT_MONEY, {"Data/Sound/Game/Shop/SE_SHOP_NOT_MONEY.mp3", 200} },

	};
}
