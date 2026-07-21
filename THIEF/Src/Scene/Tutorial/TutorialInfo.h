#pragma once
#include <string>

// 状態
namespace Tutorial
{
	enum STATE
	{
		MOVE,   // 移動方法を確認
		JUMP,	// ジャンプ方法を確認
		DASH,	// ダッシュ方法を確認
		CROUCH,	// しゃがみ方法を確認
		SLIDING,// スライディング方法を確認
		LIGHT,	// ライトを動かし方を確認
		MAP,	// マップの開き方を確認
		GRAB,	// アイテムの掴み方を確認
		RANGE,	// アイテムの動かし方を確認
		CART,	// アイテムをカートに入れる方法を確認
		DELIVER,// 納品の方法を確認
		MAX,
	};

	struct TutorialInfo
	{
		int id;
		std::string type;
		float  value;
		std::string textEN;
		std::string textJP;
	};

}
