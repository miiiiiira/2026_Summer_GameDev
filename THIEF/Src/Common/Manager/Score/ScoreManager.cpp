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

	int totalMoney = 0;

	for (Item* item : items_)
	{
		// 生存していなかったら処理を行わない
		if (!item->GetInfo().isAlive_)return;

		totalMoney += item->GetInfo().money_;
	}

	// 生存中のアイテムが目標金額より下回ってしまったら
	if (totalMoney < targetPrice_)
	{
		// ゲームオーバーにする
		SceneManager::GetInstance()->TrueGameOver();
	}
}

void ScoreManager::Draw(void)
{
	// 納品金額 / 目標金額の描画
	DrawFormatString(Application::SCREEN_SIZE_X - 150, 50, 0xffffff, "%d　／　%d", deliveryPrice_, targetPrice_);
}

void ScoreManager::Destroy()
{
	delete instance_;
	instance_ = nullptr;
}

void ScoreManager::ResetGame()
{
	totalPrice_ = deliveryPrice_ = 0;
}

ScoreManager::ScoreManager(void)
{
	totalPrice_ = deliveryPrice_ = 0;

	// 目標金額を初期化
	targetPrice_ = TARGET_PRICE;
}
