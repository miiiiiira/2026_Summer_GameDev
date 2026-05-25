#include "DeliveryLocationCollider.h"
#include "../../../Object.h"
#include "../../Transform/Transform.h"
#include "../../Stage/Stage.h"
#include "../../Item/Item.h"
#include "../../../../Common/Collision/Collision.h"

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

	// アイテムの座標
	VECTOR itemPos = stage_->GetItem()->GetTransform()->pos_;

	// アイテムのサイズ(中心から端までの半径)
	VECTOR itemSize = { stage_->GetItem()->GetInfo().collisionRadiusX_
		, stage_->GetItem()->GetInfo().collisionRadiusY_
		,stage_->GetItem()->GetInfo().collisionRadiusX_ };

	if (Collision::HitAABBs(deliveryPos, deliverySize, itemPos, itemSize))
	{
		stage_->GetItem()->SetHasTouchedDelivery(true);
	}
	else
	{
		stage_->GetItem()->SetHasTouchedDelivery(false);
	}

}
