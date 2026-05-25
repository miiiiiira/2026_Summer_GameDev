#include "DeliveryLocationCollider.h"
#include "../../../Object.h"
#include "../../Transform/Transform.h"
#include "../../Stage/Stage.h"
#include "../../Item/Item.h"

void DeliveryLocationCollider::Init(void)
{
	// ステージの確保
	stage_ = owner_->GetComponent<Stage>();
}

void DeliveryLocationCollider::Update(void)
{
	if (!stage_)return;

	if (!item_)return;

	// アイテムと納品場所の当たり判定
	ItemToDeliveryLocationCollision();
}

void DeliveryLocationCollider::ItemToDeliveryLocationCollision(void)
{
	// 納品場所の座標
	VECTOR deliveryPos = stage_->GetDeliveryPos();

	// アイテムの座標
	VECTOR itemPos = item_->GetTransform()->pos_;

	// アイテムの半径
	float itemRadX = item_->GetInfo().collisionRadiusX_;
	float itemRadY = item_->GetInfo().collisionRadiusY_;

	//auto b = aabb->transform_->pos_;

	//// XYZ軸それぞれで重なり判定
	//return (
	//	abs(a.x - b.x) <= (size_.x + aabb->size_.x) * 0.5f &&
	//	abs(a.y - b.y) <= (size_.y + aabb->size_.y) * 0.5f &&
	//	abs(a.z - b.z) <= (size_.z + aabb->size_.z) * 0.5f
	//	);
}
