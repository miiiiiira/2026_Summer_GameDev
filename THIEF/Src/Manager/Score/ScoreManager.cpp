#include "ScoreManager.h"
#include <DxLib.h>
#include "../../Object/Component/Item/Item.h"
#include "../../Scene/SceneManager.h"
#include "../../Application.h"

ScoreManager* ScoreManager::instance_ = nullptr;

void ScoreManager::Update(void)
{
	// チュートリアルなら処理を行わない
	if (SceneManager::GetInstance()->GetNowSceneTag() == TUTORIAL)return;
	
	// アイテムの中身がなかったら処理を行わない
	if (items_.empty())return;

	int totalPrice = 0;

	for (Item* item : items_)
	{
		// 生存していなかったら次のアイテムへ
		if (!item->GetInfo().isAlive_)continue;

		// 生存していたらトータル金額に加算
		totalPrice += item->GetInfo().price_;
	}

	// 生存中のアイテムの全金額が目標金額に近づいてきたら
	if (!showWarning_ && totalPrice < warningPrice_)
	{
		// 警告文表示用のフラグを立てる
		showWarning_ = true;
	}

	// 生存中のアイテムが目標金額より下回ってしまったら
	if (totalPrice < targetPrice_)
	{
		// ゲームオーバーにする
		SceneManager::GetInstance()->TrueGameOver();
		return;
	}

	// 警告文を表示中でなければ処理を行わない
	if (!showWarning_)return;

	// ボタンのアルファ値を変化させる
	if (isIncreasing_)
	{
		// 増加速度
		alpha_ += ALPHA_SPEED;

		// 増加が最大になったら減少に切り替える
		if (alpha_ >= ALPHA_MAX)
		{
			alpha_ = ALPHA_MAX;
			isIncreasing_ = false;
		}
	}
	else
	{
		// 減少速度
		alpha_ -= ALPHA_SPEED;

		// 減少が最小になったら増加に切り替える
		if (alpha_ <= ALPHA_MIN)
		{
			alpha_ = ALPHA_MIN;
			isIncreasing_ = true;
		}
	}
}

void ScoreManager::Draw(void)
{
	// 納品金額 / 目標金額の描画
	int font = Application::GetInstance()->GetFont(FONT_SIZE_20);
	
	// 基本の色(白)
	unsigned int priceCol = 0xffffff;
	// 納品金額が目標金額を超えていたら
	if (deliveryPrice_ >= targetPrice_)
	{
		// 黄色へ
		priceCol = 0xffff00;
	}

	// フォント文字列の幅を調べる
	int strWidth = GetDrawStringWidthToHandle("Score / Target", 14, font);
	// 納品金額
	DrawStringToHandle(Application::SCREEN_SIZE_X - strWidth , 30,"Score / Target", priceCol, font);
	// 納品金額(実際の数値)
	strWidth = GetDrawFormatStringWidthToHandle(font, "%d　/　%d", deliveryPrice_, targetPrice_);
	DrawFormatStringToHandle(Application::SCREEN_SIZE_X - strWidth , 70, priceCol, font,
		"%d　/　%d",
		deliveryPrice_, targetPrice_);


	// 基本の色(白)
	unsigned int cartPriceCol = 0xffffff;
	// カート内の金額が目標金額を超えていたら
	if (cartPrice_ >= targetPrice_)
	{
		// 黄色へ
		cartPriceCol = 0xffff00;
	}

	// フォント文字列の幅を調べる
	strWidth = GetDrawStringWidthToHandle("CartTotal",9,font);
	// カート内の金額
	DrawStringToHandle(Application::SCREEN_SIZE_X - strWidth , 130, "CartTotal", cartPriceCol, font);
	// カート内の金額(実際の数値)
	strWidth = GetDrawFormatStringWidthToHandle(font, "%d", cartPrice_);
	DrawFormatStringToHandle(Application::SCREEN_SIZE_X - strWidth , 170, cartPriceCol, font,
		"%d",
		cartPrice_);


	// 生存中のアイテムが目標金額より下回ってしまったら
	if (showWarning_)
	{
		// フォント文字列の幅を調べる
		strWidth = GetDrawStringWidthToHandle( "Target Score at Risk!",22, font);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha_));

		// 警告文を付ける
		DrawStringToHandle(Application::SCREEN_SIZE_X - strWidth, 210,"Target Score at Risk!", 0xff0000, font);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void ScoreManager::Destroy()
{
	DeleteInstance();
}

void ScoreManager::ResetGame()
{
	deliveryPrice_ = 0;
	showWarning_ = false;
	alpha_ = ALPHA_MAX;
	isIncreasing_ = false;
}

void ScoreManager::ResetTotalPrice(void)
{
	totalPrice_ = 0;
}

void ScoreManager::SetItems(std::vector<Item*> items)
{
	// アイテムのポインタをもらう
	items_ = items;

	// アイテムの中身がなかったら処理を行わない
	if (items_.empty())return;

	if (SceneManager::GetInstance()->GetNowSceneTag() == TUTORIAL)
	{
		// チュートリアル中は目標金額を強制的に1とする
		cartPrice_ = 0;
		targetPrice_ = 1;
		warningPrice_ = 0;
	}
	else
	{
		cartPrice_ = 0;
		targetPrice_ = 0;
		warningPrice_ = 0;

		int allPrice = 0;

		// アイテムたちの全金額を調べる
		for (Item* item : items_)
		{
			allPrice += item->GetInfo().price_;
		}

		//	目標金額を全金額の50%に設定
		targetPrice_ = static_cast<int>(allPrice * TARGET_PRICE_RATIO);
		// 100円以下は切り捨て
		int price = targetPrice_ % 100;
		// 目標金額を設定
		targetPrice_ -= price;

		//	警告文を出す目安金額を全金額の70%に設定
		warningPrice_ = static_cast<int>(allPrice * SHOW_WARNING_PRICE_RATIO);

#ifdef _DEBUG
		// デバック時は目標金額を強制的に1とする
		targetPrice_ = 1;
#endif // _DEBUG

	}
}

ScoreManager::ScoreManager(void)
{
	// 金額類を全初期化
	ResetGame();
	ResetTotalPrice();

	// 目標金額を初期化
	targetPrice_ = 0;
}
