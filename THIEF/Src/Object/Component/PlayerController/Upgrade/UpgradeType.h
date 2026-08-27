#pragma once

namespace
{
	// アップグレードの種類
	enum class PLAYER_UPGRADE_TYPE
	{
		HP_UP,			// 最大HPを上げる
		STAMINA_UP,		// スタミナを上げる
		DASH_SPEED_UP,	// ダッシュ時の移動速度を上げる
		RANGE_UP,		// 掴める距離を上げる
		JUMP_NUM_UP,	// ジャンプの回数を増やす
		HEAL_HP_25,		// HPを25回復する
		HEAL_HP_50,		// HPを50回復する

		MAX
	};

}