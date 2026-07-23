#include "ScoreManager.h"
#include <DxLib.h>
#include "../../../Object/Component/Item/Item.h"
#include "../../../Scene/SceneManager.h"
#include "../../../Application.h"

ScoreManager* ScoreManager::instance_ = nullptr;

void ScoreManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ScoreManager();
	}
}

ScoreManager& ScoreManager::GetInstance(void)
{
	return *instance_;
}

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

		totalPrice += item->GetInfo().price_;
	}

	// 生存中のアイテムの全金額が目標金額に近づいてきたら
	if (!showWarning_ && totalPrice < warningPrice_)
	{
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
		alpha_ += ALPHA_SPEED; // 増加速度

		// 増加が最大になったら減少に切り替える
		if (alpha_ >= ALPHA_MAX)
		{
			alpha_ = ALPHA_MAX;
			isIncreasing_ = false;
		}
	}
	else
	{
		alpha_ -= ALPHA_SPEED; // 減少速度

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
	int font = Application::GetInstance()->GetFont();
	int strWidth = GetDrawFormatStringWidthToHandle(font, "%d　/　%d", deliveryPrice_, targetPrice_);
	
	unsigned int priceCol = 0xffffff;

	if (deliveryPrice_ >= targetPrice_)
	{
		priceCol = 0xffff00;
	}

	DrawFormatStringToHandle(Application::SCREEN_SIZE_X - strWidth , 50, priceCol, font,
		"%d　/　%d",
		deliveryPrice_, targetPrice_);

	// 生存中のアイテムが目標金額より下回ってしまったら
	if (showWarning_)
	{
		strWidth = GetDrawStringWidthToHandle( "Target Score at Risk!",22, font);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha_));
		// 警告文を付ける
		DrawStringToHandle(Application::SCREEN_SIZE_X - strWidth, 90,"Target Score at Risk!", 0xff0000, font);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void ScoreManager::Destroy()
{
	delete instance_;
	instance_ = nullptr;
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
	items_ = items;

	// アイテムの中身がなかったら処理を行わない
	if (items_.empty())return;

	if (SceneManager::GetInstance()->GetNowSceneTag() == TUTORIAL)
	{
		targetPrice_ = 1;
		warningPrice_ = 0;
	}
	else
	{
		int allPrice = 0;
		targetPrice_ = 0;
		warningPrice_ = 0;
		for (Item* item : items_)
		{
			allPrice += item->GetInfo().price_;
		}

		//	目標金額を50%にする
		targetPrice_ = allPrice * TARGET_PRICE_RATIO;

		// 100円以下は切り捨て
		int price = targetPrice_ % 100;
		// 目標金額を設定
		targetPrice_ -= price;

		//	警告文を出す目安金額を70%にする
		warningPrice_ = allPrice * SHOW_WARNING_PRICE_RATIO;

#ifdef _DEBUG
		targetPrice_ = 1;
#endif // _DEBUG

	}
}

ScoreManager::ScoreManager(void)
{
	ResetGame();
	ResetTotalPrice();

	// 目標金額を初期化
	targetPrice_ = 0;
}
