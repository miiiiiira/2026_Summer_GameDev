#include "ItemCollider.h"
#include "../../../Object.h"
#include "../../PlayerController/PlayerController.h"
#include "../../../../Common/Transform/MatrixUtility.h"

void ItemCollider::Init(void)
{
	// アイテムの確保
	item_ = owner_->GetComponent<Item>();

	// Transform取得
	transform_ = owner_->GetComponent<Transform>();

	// CapsuleCollider取得
	capsule_ = owner_->GetComponent<CapsuleCollider>();
}

void ItemCollider::Update(void)
{
	if (!transform_) return;

	//if (!capsule_) return;

	if (!player_) return;

	// プレイヤーの掴み機能との当たり判定
	PlayerGrabCollision();

	// ステージとの当たり判定
	StageCollision();
}

void ItemCollider::PlayerGrabCollision(void)
{
	// プレイヤーが何かを掴んでいる状態だったら処理を行わない
	if (player_->grabState_ == GrasbbingState::IS_GRABBING)return;

	// アイテムのモデルIDを取得
	int itemModelId = item_->GetModelID();

	// 線分の上座標
	VECTOR topPos = item_->GetLineStartPos();

	// 線分の下座標
	VECTOR downPos = item_->GetLineEndPos();

	// 線分とモデルの衝突判定
	MV1_COLL_RESULT_POLY hitResult =
		MV1CollCheck_Line(itemModelId, -1, topPos, downPos);

	// 当たっているかつ、掴もうとしていたら
	if (hitResult.HitFlag && player_->GetGrabbingState() == GrasbbingState::TRY_GRABBING)
	{
		// カメラと当たった場所の距離を求める
		float distance = item_->GetCameraDistance(hitResult.HitPosition);

		// 距離が最低距離値よりも小さかったら最低距離値にする
		if (distance < PlayerController::MIN_RENGE)distance = PlayerController::MIN_RENGE;

		// アイテムの追従モードをオンにする
		item_->StartGrabbing({ 0,0,distance });
		// 掴み状態にする
		player_->StartGrabbing(distance);
		player_->SetItemPoint(item_);
	}
}

void ItemCollider::StageCollision(void)
{
}
