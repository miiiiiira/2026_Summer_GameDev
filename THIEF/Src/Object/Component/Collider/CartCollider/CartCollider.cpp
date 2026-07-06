#include "CartCollider.h"
#include "../../../Object.h"
#include "../../Cart/Cart.h"
#include "../../Stage/Stage.h"
#include "../../PlayerController/PlayerController.h"
#include "../../../../Common/Crosshair/Crosshair.h"

void CartCollider::Init(void)
{
	cart_ = owner_->GetComponent<Cart>();
}

void CartCollider::Update(void)
{
	if (!cart_) return;

	if (!stage_) return;

	if (!player_) return;

	if (!crosshair_) return;

	// カートとプレイヤー掴み機能との当たり判定
	CartToPlayerGrabbingCollision();

}

void CartCollider::Draw2D(void)
{
}

void CartCollider::CartToPlayerGrabbingCollision(void)
{
	// プレイヤーが何かを掴んでいる状態だったら処理を行わない
	if (player_->GetGrabbingState() == GRABBING_STATE::IS_GRABBING) return;

	// 線分の上座標
	VECTOR lineStartPos = player_->GetLineStartPos();

	// 線分の下座標
	VECTOR lineEndPos = player_->GetLineEndPos();

	// 線分とカートモデル衝突判定
	MV1_COLL_RESULT_POLY itemHitResult = MV1CollCheck_Line(cart_->GetModelId(), 1, lineStartPos, lineEndPos);
	// 線分と当たっていないなら処理をしない
	if (!itemHitResult.HitFlag)return;

	// カメラとアイテムに線分をつなげてステージに当たっているか
	// 線分とステージモデルの衝突判定
	MV1_COLL_RESULT_POLY stageHitResult =
		MV1CollCheck_Line(stage_->GetModelId(), -1, lineStartPos, itemHitResult.HitPosition);

	// ステージに当たっていたら
	if (stageHitResult.HitFlag)return;

	// 当たっている
	// クロスヘアの種類を掴めるに変更
	crosshair_->ChangeCrosshair(CROSSHAIR_TYPE::CAN_GRAB);

	// 掴もうとしていたら
	if (player_->GetGrabbingState() == GRABBING_STATE::TRY_GRABBING)
	{
		// アイテムの追従モードをオンにする
		cart_->StartGrabbing();
		// 掴み状態にする
		player_->StartGrabbing(0);
		player_->SetGrabObject(cart_);

		// クロスヘアの種類を掴んでいるに変更
		crosshair_->ChangeCrosshair(CROSSHAIR_TYPE::GRABBING);
	}
}
