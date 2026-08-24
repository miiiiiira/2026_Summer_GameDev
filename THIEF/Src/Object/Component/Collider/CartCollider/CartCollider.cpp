#include "CartCollider.h"
#include "../../../Object.h"
#include "../../Cart/Cart.h"
#include "../../Stage/Stage.h"
#include "../../PlayerController/PlayerController.h"
#include "../../Transform/Transform.h"
#include "../../Crosshair/Crosshair.h"
#include "../../../../Common/CameraUtility/CameraUtility.h"
#include "../../../../Common/Collision/Collision.h"
#include "../../../../Manager/PlayerStatus/PlayerStatusManager.h"
#include "../../../../Application.h"
#include "../../../../Manager/System/SystemManager.h"
#include "../../../../Manager/Input/InputManager.h"

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

	// カートの取っ手座標
	VECTOR cartHandlePos = MV1GetFramePosition(cart_->GetModelId(), 1);

	// 線分の上座標
	VECTOR lineStartPos = player_->GetLineStartPos();

	// 線分の下座標
	VECTOR lineEndPos = player_->GetLineEndPos();

	// 当たった座標
	VECTOR hitPos = cartHandlePos;

	// 掴めるか
	bool isGrab = true;

	// 線分とカートの衝突判定
	MV1_COLL_RESULT_POLY cartHitResult = MV1CollCheck_Line(cart_->GetModelId(), 1, lineStartPos, lineEndPos);

	// 線分と当たっていない
	if (!cartHitResult.HitFlag)
	{
		// 掴めない
		isGrab = false;
	}
	else
	{
		// 当たった座標を取る
		hitPos = cartHitResult.HitPosition;
	}

	// パッドかつ掴めない判定が出ていたら
	if (InputManager::GetInstance()->GetActiveDevice() == InputManager::ActiveDevice::PAD
		&& !isGrab)
	{
		// プレイヤーの掴める距離にカートが入っていなければ処理を行わない
		if (!Collision::HitSpherePoint(
			player_->GetTransform()->pos_,
			PlayerStatusManager::GetInstance().GetPlayerStatus().rangeMax_,
			cartHandlePos))return;

		// 場所が視界内に入っていないため処理を行わない
		if (CheckCameraViewClip(cartHandlePos))return;

		// ワールド座標をスクリーン座標にする
		VECTOR pos = ConvWorldPosToScreenPos(cartHandlePos);
		Vector2 screenPos = { pos.x,pos.y };
		// スクリーン上で掴み可能な範囲
		Vector2 checkBoxPos = { Application::SCREEN_SIZE_X / 2 - SystemManager::CONTROLLER_GRAB_SCREEN_RANGE_RAD ,
			Application::SCREEN_SIZE_Y / 2 - SystemManager::CONTROLLER_GRAB_SCREEN_RANGE_RAD };

		// 掴み可能な範囲に入っている
		if (Collision::HitPoint2Box(
			screenPos,
			checkBoxPos,
			SystemManager::CONTROLLER_GRAB_SCREEN_RANGE,
			SystemManager::CONTROLLER_GRAB_SCREEN_RANGE))
		{
			// 掴める
			isGrab = true;
		}
	}

	// 掴める範囲に無いため処理を行わない
	if (!isGrab)return;

	// カメラとカートに線分をつなげてステージに当たっているか
	// 線分とステージモデルの衝突判定
	MV1_COLL_RESULT_POLY stageHitResult =
		MV1CollCheck_Line(stage_->GetModelId(), -1, lineStartPos, hitPos);

	// ステージに当たっていたら
	if (stageHitResult.HitFlag)return;

	// 当たっている
	// クロスヘアの種類を掴めるに変更
	crosshair_->ChangeCrosshair(CROSSHAIR_CAN_GRAB);

	// 掴もうとしていたら
	if (player_->GetGrabbingState() == GRABBING_STATE::TRY_GRABBING)
	{
		// カートの追従モードをオンにする
		cart_->StartGrabbing({ 0.0f,CameraUtility::GetCameraPos().y ,PlayerController::CART_DISTANCE });
		// 掴み状態にする
		player_->StartGrabbing(0);
		player_->SetGrabObject(cart_);

		// クロスヘアの種類を掴んでいるに変更
		crosshair_->ChangeCrosshair(CROSSHAIR_GRABBING);
	}
}
