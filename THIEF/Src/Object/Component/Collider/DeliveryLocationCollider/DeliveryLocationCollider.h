#pragma once

#include "../../Component.h"

// 前方宣言
class Item;
class Stage;

class DeliveryLocationCollider:public Component
{
public:
	void Init(void) override;		// 初期化
	void Update(void) override;		// 更新

	// アイテム設定
	void SetItem(Item* item) { item_ = item; }

private:

	// アイテムと納品場所の当たり判定
	void ItemToDeliveryLocationCollision(void);

private:

	// ステージ
	Stage* stage_ = nullptr;

	// アイテム
	Item* item_ = nullptr;
};

