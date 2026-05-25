#include "Stage.h"

#include <DxLib.h>

#include "../Render/Render3D.h"
#include "../Collider/DeliveryLocationCollider/DeliveryLocationCollider.h"
#include "../../Object.h"

void Stage::Init()
{
	// オーナーから3D描画コンポーネントを取得
	auto render = owner_->GetComponent<Render3D>();
	if (!render) return;

	// モデルIDを取得
	modelId_ = render->GetHandle();

	// オーナーからTransformを取得
	auto trans = owner_->GetComponent<Transform>();

	// 座標の更新
	MV1SetPosition(modelId_, trans->pos_);

	// 衝突情報構築
	MV1SetupCollInfo(modelId_, -1);

	// ステージの座標
	deliveryPos_ = trans->pos_;
	deliveryPos_.y += DELIVERY_LOCATION_SIZE_HIG;
	deliveryPos_.z += 500.0f;
}

void Stage::Draw(void)
{
#ifdef _DEBUG
	DebugDraw();
#endif // _DEBUG
}

Transform* Stage::GetTransform()
{
	return owner_->GetComponent<Transform>();
}

void Stage::SetItem(Item* item)
{
	item_ = item;
}

VECTOR Stage::GetDeliveryPos(void)
{
	return deliveryPos_;
}

Item* Stage::GetItem(void)
{
	return item_;
}

VECTOR Stage::ToWorldPos(VECTOR local)
{
	auto trans = owner_->GetComponent<Transform>();
	return VAdd(local, trans->pos_);
}

VECTOR Stage::ToLocalPos(VECTOR world)
{
	auto trans = owner_->GetComponent<Transform>();
	return VSub(world, trans->pos_);
}

void Stage::DebugDraw(void)
{
	VECTOR startPos, endPos;
	startPos = endPos = deliveryPos_;

	startPos.x -= DELIVERY_LOCATION_SIZE_WID;
	startPos.y -= DELIVERY_LOCATION_SIZE_HIG;
	startPos.z -= DELIVERY_LOCATION_SIZE_WID;

	endPos.x += DELIVERY_LOCATION_SIZE_WID;
	endPos.y += DELIVERY_LOCATION_SIZE_HIG;
	endPos.z += DELIVERY_LOCATION_SIZE_WID;

	DrawCube3D(startPos, endPos, 0x0000ff, 0x0000ff, false);
}