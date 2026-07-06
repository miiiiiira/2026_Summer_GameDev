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
	if (!showWarning_ && totalPrice < itemAllPrice_ * 0.7f)
	{
		showWarning_ = true;
	}

	//// 生存中のアイテムが目標金額より下回ってしまったら
	//if (totalPrice < targetPrice_)
	//{
	//	// ゲームオーバーにする
	//	SceneManager::GetInstance()->TrueGameOver();
	//}
}

void ScoreManager::Draw(void)
{
	// 納品金額 / 目標金額の描画
	int font = Application::GetInstance()->GetFont();
	int strWidth = GetDrawFormatStringWidthToHandle(font, "%d　/　%d", deliveryPrice_, targetPrice_);
	DrawFormatStringToHandle(Application::SCREEN_SIZE_X - strWidth , 50, 0xffffff, font,
		"%d　/　%d",
		deliveryPrice_, targetPrice_);

	// 生存中のアイテムが目標金額より下回ってしまったら
	if (showWarning_)
	{
		strWidth = GetDrawStringWidthToHandle( "目標金額を下回る可能性があります。ご注意ください。",25, font);
		DrawStringToHandle(Application::SCREEN_SIZE_X - strWidth, 65,"目標金額を下回る可能性があります。ご注意ください。", 0xff0000, font);
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

	int allPrice = 0;
	itemAllPrice_ = 0;
	for (Item* item : items_)
	{
		allPrice += item->GetInfo().price_;
	}

	itemAllPrice_ = allPrice;

	// 60パーセントにする
	allPrice *= TARGET_PRICE_RATIO;

	// 100円以下は切り捨て
	int price = allPrice % 100;
	allPrice -= price;

	// 目標金額を設定
	targetPrice_ = allPrice;
}

ScoreManager::ScoreManager(void)
{
	ResetGame();
	ResetTotalPrice();

	// 目標金額を初期化
	targetPrice_ = 0;
}
