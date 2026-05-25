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
	void Draw(void) override;		// 描画

private:

	// アイテムと納品場所の当たり判定
	void ItemToDeliveryLocationCollision(void);

private:

	// ステージ
	Stage* stage_ = nullptr;

	void DebugDraw(void);
};

