#include "DeliveryLocationCollider.h"
#include "../../../Object.h"
#include "../../Transform/Transform.h"
#include "../../Stage/Stage.h"
#include "../../Item/Item.h"
#include "../../../Score/ScoreManager.h"
#include "../../../../Common/Collision/Collision.h"
#include "../../../../Application.h"

void DeliveryLocationCollider::Init(void)
{
	// ステージの確保
	stage_ = owner_->GetComponent<Stage>();
}

void DeliveryLocationCollider::Update(void)
{
	if (!stage_)return;

	if (!stage_->GetItem())return;

	// アイテムと納品場所の当たり判定
	ItemToDeliveryLocationCollision();
}

void DeliveryLocationCollider::Draw(void)
{
	// 納品済みの金額を確認
	int deliveryPrice = ScoreManager::GetInstance().GetDeliveryPrice();

	// 目標金額を確認
	int targetPrice = ScoreManager::GetInstance().GetTargetPrice();

	DrawFormatString(Application::SCREEN_SIZE_X - 200, 50, 0xffffff, "%d　／　%d", deliveryPrice, targetPrice);

	if (!stage_)return;

	if (!stage_->GetItem())return;

#ifdef _DEBUG
	DebugDraw();
#endif // _DEBUG
}

void DeliveryLocationCollider::DebugDraw(void)
{
	// 納品場所の座標
	VECTOR deliveryPos = stage_->GetDeliveryPos();

	// 納品場所のサイズ
	VECTOR deliverySize = { Stage::DELIVERY_LOCATION_SIZE_WID
		,Stage::DELIVERY_LOCATION_SIZE_HIG
		,Stage::DELIVERY_LOCATION_SIZE_WID };

	// アイテムの座標
	VECTOR itemPos = stage_->GetItem()->GetTransform()->pos_;

	// アイテムのサイズ(中心から端までの半径)
	VECTOR itemSize = { stage_->GetItem()->GetInfo().collisionRadiusX_
		, stage_->GetItem()->GetInfo().collisionRadiusY_
		,stage_->GetItem()->GetInfo().collisionRadiusX_ };

	if (Collision::HitAABBs(deliveryPos, deliverySize, itemPos, itemSize))
	{
		DrawString(20, 400, "納品場所に入った！", 0xffffff);
	}
	else
	{
		DrawString(20, 400, "納品場所に入ってない…", 0xffffff);
	}
}

void DeliveryLocationCollider::ItemToDeliveryLocationCollision(void)
{
	// 納品場所の座標
	VECTOR deliveryPos = stage_->GetDeliveryPos();

	// 納品場所のサイズ
	VECTOR deliverySize = { Stage::DELIVERY_LOCATION_SIZE_WID
		,Stage::DELIVERY_LOCATION_SIZE_HIG
		,Stage::DELIVERY_LOCATION_SIZE_WID };

	// 判定をするアイテムのポインタ
	Item* item = stage_->GetItem();

	// アイテムの座標
	VECTOR itemPos = stage_->GetItem()->GetTransform()->pos_;

	// アイテムのサイズ(中心から端までの半径)
	VECTOR itemSize = { stage_->GetItem()->GetInfo().collisionRadiusX_
		, stage_->GetItem()->GetInfo().collisionRadiusY_
		,stage_->GetItem()->GetInfo().collisionRadiusX_ };

	// 当たっているかつ、納品場所に入っていないフラグが立っていたら
	if (Collision::HitAABBs(deliveryPos, deliverySize, itemPos, itemSize)
		&& !item->GetInfo().hasTouchedDeliveryLocation_)
	{
		// アイテム事体に納品場所にはいっていることを伝える
		item->SetHasTouchedDelivery(true);

		// そのアイテム分納品金額に足す
		ScoreManager::GetInstance().AddDeliveryPrice(item->GetInfo().money_);
	}
	// 当たっていないかつ、納品場所に入っているフラグが立っていたら
	else if(!Collision::HitAABBs(deliveryPos, deliverySize, itemPos, itemSize)
		&&item->GetInfo().hasTouchedDeliveryLocation_)
	{
		// アイテム事体に納品場所にはいっていないことを伝える
		stage_->GetItem()->SetHasTouchedDelivery(false);

		// そのアイテム分納品金額から引く
		ScoreManager::GetInstance().AddDeliveryPrice(-item->GetInfo().money_);
	}

}
