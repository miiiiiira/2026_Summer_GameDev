#include "ItemCollider.h"
#include "../../../Object.h"
#include "../../PlayerController/PlayerController.h"
#include "../../Stage/Stage.h"
#include "../../Cart/Cart.h"
#include "../../Item/Item.h"
#include "../../Wisp/Wisp.h"
#include "../../../../Common/Transform/MatrixUtility.h"
#include "../../../../Common/CameraUtility/CameraUtility.h"
#include "../../../../Common/Manager/Audio/AudioManager.h"
#include "../../../../Common/Collision/Collision.h"
#include "../../../../Common/Crosshair/Crosshair.h"
#include "../../../../Application.h"

#include <algorithm>

void ItemCollider::Init(void)
{
	// アイテムの確保
	item_ = owner_->GetComponent<Item>();
}

void ItemCollider::Update(void)
{

	if (!item_) return;

	// アイテムが生存中でなければ処理を行わない
	if (!item_->GetInfo().isAlive_)return;

	if (!player_) return;

	if (!stage_) return;

	// カメラレイとの当たり判定
	CameraRayCollision();

	// プレイヤーの掴み機能との当たり判定
	PlayerGrabCollision();

	// アイテムとカートの当たり判定
	ItemToCartCollision();

	// アイテムがカートに入っているかの当たり判定
	ItemInCartCollision();

	// ステージとの当たり判定
	StageCollision();
}

void ItemCollider::CameraRayCollision(void)
{
	// すでに見つけていたら処理を行わない
	if (item_->GetInfo().isFound_)return;

	// 線分の上座標
	VECTOR lineStartPos = player_->GetLineStartPos();

	//プレイヤーから一定距離内に入っているか
	if (!Collision::HitSpherePoint(
		lineStartPos,
		PlayerController::PLAYER_ITEM_SEARCH_RADIUS,
		item_->GetTransform()->pos_))return;

	// ダメージの場所が視界内に入っていないのであれば処理をスキップ
	if (CheckCameraViewClip(item_->GetTransform()->pos_))return;

	// カメラとアイテムに線分をつなげてステージに当たっているか
	// 線分とステージモデルの衝突判定
	MV1_COLL_RESULT_POLY stageHitResult =
		MV1CollCheck_Line(stage_->GetModelId(), -1, lineStartPos, item_->GetTransform()->pos_);

	// ステージに当たっていたら
	if (stageHitResult.HitFlag)return;

	// 当たっていたら見つけた判定にする
	item_->TrueIsFound();

}

void ItemCollider::PlayerGrabCollision(void)
{
	// プレイヤーが何かを掴んでいる状態だったら処理を行わない
	if (player_->GetGrabbingState() == GRABBING_STATE::IS_GRABBING) return;

	// 線分の上座標
	VECTOR lineStartPos = player_->GetLineStartPos();

	// 線分の下座標
	VECTOR lineEndPos = player_->GetLineEndPos();

	// 線分とアイテムモデルの衝突判定
	MV1_COLL_RESULT_POLY itemHitResult = MV1CollCheck_Line(item_->GetModelID(), -1, lineStartPos, lineEndPos);
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
		// カメラと当たった場所の距離を求める
		float distance = item_->GetCameraDistance();

		// 距離が最低距離値よりも小さかったら最低距離値にする
		if (distance < PlayerController::MIN_RENGE)distance = PlayerController::MIN_RENGE;

		// アイテムの追従モードをオンにする
		item_->StartGrabbing({ 0,0,distance });
		// 掴み状態にする
		player_->StartGrabbing(distance);
		player_->SetGrabObject(item_);

		// クロスヘアの種類を掴んでいるに変更
		crosshair_->ChangeCrosshair(CROSSHAIR_TYPE::GRABBING);
	}
}

void ItemCollider::ItemToCartCollision(void)
{
	if (!cart_) return;

	// 接触したかどうか
	bool isHitCart = false;

	// 現在の座標
	VECTOR itemCurrentPos = item_->GetTransform()->pos_;
	// 前回の座標取得
	VECTOR itemPrevPos = item_->GetTransform()->prevPos_;
	// アイテム移動量(前回の座標と現在の座標の差分)
	VECTOR itemMove = VSub(itemCurrentPos, itemPrevPos);

	// 現在の座標
	VECTOR cartCurrentPos = cart_->GetTransform()->pos_;
	// 前回の座標取得
	VECTOR cartPrevPos = cart_->GetTransform()->prevPos_;
	// カート移動量
	VECTOR cartMove = VSub(cartCurrentPos, cartPrevPos);

	// アイテムのトータルの移動量  (自身の移動 - カートの移動)
	VECTOR move = VSub(itemMove, cartMove);

	// 衝突判定開始座標
	VECTOR pos = itemPrevPos;

	// 衝突とスライドを繰り返す
	for (int bounce = 0; bounce < MAX_BOUNCE; bounce++)
	{
		// 残り移動量の長さ
		float length = VSize(move);

		// ほぼ移動していないなら終了
		if (length < 0.01f)
			break;

		// カプセル半径
		float radius = item_->GetInfo().collisionRadiusX_;

		// 高速移動時のすり抜け防止のため、
		// 移動経路を細かく分割して判定する
		int stepCount = (int)(length / (radius * 0.05f)) + 1;

		// 分割数の上限・下限を設定
		stepCount = std::clamp(stepCount, 1, 32);

		// 1ステップ当たりの移動量
		VECTOR stepMove = VScale(move, 1.0f / static_cast<float>(stepCount));

		// 衝突情報
		bool hit = false;
		VECTOR hitNormal = VGet(0, 0, 0);

		// 衝突していない最後の座標
		VECTOR safePos = pos;

		// 衝突したステップ番号
		int hitStep = stepCount;

		// 経路を少しずつ進めながら判定
		for (int step = 0; step < stepCount; step++)
		{
			// 次に移動する座標
			VECTOR nextPos = VAdd(safePos, stepMove);

			// カプセル生成
			VECTOR capStart = nextPos;
			capStart.y -= item_->GetInfo().collisionRadiusY_;
			VECTOR capEnd = nextPos;
			capEnd.y += item_->GetInfo().collisionRadiusY_;

			// ステージとカプセルの衝突判定
			auto result =
				MV1CollCheck_Capsule(
					cart_->GetModelId(),
					-1,
					capStart,
					capEnd,
					radius);

			float bestPush = 0.0f;
			VECTOR bestNormal = VGet(0, 0, 0);

			bool collision = false;

			// ヒットしたポリゴンを調べる
			for (int i = 0; i < result.HitNum; i++)
			{
				auto& poly = result.Dim[i];

				// ポリゴン法線
				VECTOR normal = VNorm(poly.Normal);

				// このステップの進行方向と法線から、正面衝突している度合いを求める
				float push = -VDot(VNorm(stepMove), normal);

				// 背面や平行な面は無視
				if (push <= 0.0f)
					continue;

				// 最も正面から当たっている面を採用
				if (push > bestPush)
				{
					bestPush = push;
					bestNormal = normal;
				}

				collision = true;
			}

			// 衝突結果を解放
			MV1CollResultPolyDimTerminate(result);

			// 衝突したら探索終了
			if (collision)
			{
				hit = true;
				hitNormal = bestNormal;
				hitStep = step;
				break;
			}

			// この位置までは移動しても大丈夫
			safePos = nextPos;
		}

		// 最後まで衝突しなかった
		if (!hit)
		{
			pos = VAdd(pos, move);
			break;
		}

		isHitCart = true;

		// 衝突していない最後の座標へ戻す
		pos = safePos;

		// 少しだけ法線の方向へ押し出してめり込みを防止する
		pos = VAdd(pos, VScale(hitNormal, SKIN));

		// 床に当たった
		if (hitNormal.y > FLOOR_NORMAL_Y)
		{
			// 下方向の加速度を0にする
			item_->SetVelocityYZero();
		}

		// 衝突後に残っている移動割合
		float remainRatio = (float)(stepCount - hitStep) / stepCount;
		VECTOR remainMove = VScale(move, remainRatio);

		// 壁スライド処理
		// 法線方向成分を除去して壁に沿って移動させる
		float dot = VDot(remainMove, hitNormal);

		if (dot < 0.0f)
		{
			remainMove = VSub(remainMove, VScale(hitNormal, dot));
		}

		// 次の反復で残り移動量を処理
		move = remainMove;
	}

	if (isHitCart)
	{
		// 最終位置を反映
		item_->SetPos(VAdd(VAdd(pos, cartMove), { 0.0f,-cart_->GetVelocityY() ,0.0f }));
		item_->SetPrevPos(item_->GetTransform()->pos_);
		item_->OnFloor();
	}
}

void ItemCollider::ItemInCartCollision(void)
{
	// カートの座標と回転角
	VECTOR cartPos = cart_->GetTransform()->pos_;
	cartPos.y += Cart::CART_SIZE_HIG_RAD;

	// カートのY軸回転角
	float cartAngleY = cart_->GetTransform()->angle_.y; 

	// カートのサイズ
	VECTOR cartSize = VGet(Cart::CART_SIZE_WID_RAD, Cart::CART_SIZE_HIG_RAD, Cart::CART_SIZE_DEPTH_RAD);

	// アイテムの座標
	VECTOR itemPos = item_->GetTransform()->pos_;

	// アイテムを「カートを中心としたローカル座標」に変換し、逆回転させる
	VECTOR localItemPos = VSub(itemPos, cartPos);
	// 逆方向に回転させる行列
	MATRIX invMat = MGetRotY(-cartAngleY); 
	localItemPos = VTransform(localItemPos, invMat);

	// アイテムのサイズ
	VECTOR itemSize = { item_->GetInfo().collisionRadiusX_
			, item_->GetInfo().collisionRadiusY_
			,item_->GetInfo().collisionRadiusX_ };

	// カートが真っ直ぐな状態（ローカル空間）で、アイテムが箱の中にあるか判定
	bool isHit = (abs(localItemPos.x) <= (cartSize.x + itemSize.x) &&
		abs(localItemPos.y) <= (cartSize.y + itemSize.y) &&
		abs(localItemPos.z) <= (cartSize.z + itemSize.z));

	// 当たっているかつ、カートに入っていないフラグが立っていたら
	if (isHit && !item_->GetInfo().hasTouchedCart_)
	{
		AudioManager::GetInstance()->PlaySE(SoundID::SE_DELIVERY_ITEM_ON);
		item_->SetHasTouchedCart(true);
	}
	// 当たっていないかつ、カートに入っているフラグが立っていたら
	else if (!isHit && item_->GetInfo().hasTouchedCart_)
	{
		item_->SetHasTouchedCart(false);
	}

}

void ItemCollider::StageCollision(void)
{
	// 接触したかどうか
	bool isHitStage = false;

	// 現在の座標
	VECTOR currentPos = item_->GetTransform()->pos_;

	// 前回の座標取得
	VECTOR prevPos = item_->GetTransform()->prevPos_;

	// 移動量(前回の座標と現在の座標の差分)
	VECTOR move = VSub(currentPos, prevPos);

	// 衝突判定開始座標
	VECTOR pos = prevPos;

	// 衝突とスライドを繰り返す
	for (int bounce = 0; bounce < MAX_BOUNCE; bounce++)
	{
		// 残り移動量の長さ
		float length = VSize(move);

		// ほぼ移動していないなら終了
		if (length < 0.01f)
			break;

		// カプセル半径
		float radius = item_->GetInfo().collisionRadiusX_;

		// 高速移動時のすり抜け防止のため、
		// 移動経路を細かく分割して判定する
		int stepCount = (int)(length / (radius * 0.1f)) + 1;

		// 分割数の上限・下限を設定
		stepCount = std::clamp(stepCount, 1, 32);

		// 1ステップ当たりの移動量
		VECTOR stepMove = VScale(move, 1.0f / static_cast<float>(stepCount));

		// 衝突情報
		bool hit = false;
		VECTOR hitNormal = VGet(0, 0, 0);

		// 衝突していない最後の座標
		VECTOR safePos = pos;

		// 衝突したステップ番号
		int hitStep = stepCount;

		// 経路を少しずつ進めながら判定
		for (int step = 0; step < stepCount; step++)
		{
			// 次に移動する座標
			VECTOR nextPos = VAdd(safePos, stepMove);

			// カプセル生成
			VECTOR capStart = nextPos;
			capStart.y -= item_->GetInfo().collisionRadiusY_;
			VECTOR capEnd = nextPos;
			capEnd.y += item_->GetInfo().collisionRadiusY_;

			// ステージとカプセルの衝突判定
			auto result =
				MV1CollCheck_Capsule(
					stage_->GetModelId(),
					-1,
					capStart,
					capEnd,
					radius);

			VECTOR normalSum = VGet(0, 0, 0);
			float totalWeight = 0.0f;

			bool collision = false;

			// ヒットしたポリゴンを調べる
			for (int i = 0; i < result.HitNum; i++)
			{
				auto& poly = result.Dim[i];

				// ポリゴン法線
				VECTOR normal = VNorm(poly.Normal);

				// このステップの進行方向と法線から、正面衝突している度合いを求める
				float push = -VDot(VNorm(stepMove), normal);

				// 背面や平行な面は無視
				if (push <= 0.0f)
					continue;

				normalSum = VAdd(normalSum, VScale(normal, push));

				totalWeight += push;

				collision = true;
			}

			// 衝突結果を解放
			MV1CollResultPolyDimTerminate(result);

			// 衝突したら探索終了
			if (collision && totalWeight > 0.0f)
			{
				hit = true;
				hitNormal = VNorm(VScale(normalSum, 1.0f / totalWeight));
				hitStep = step;
				break;
			}

			// この位置までは移動しても大丈夫
			safePos = nextPos;
		}

		// 最後まで衝突しなかった
		if (!hit)
		{
			pos = VAdd(pos, move);
			break;
		}

		isHitStage = true;

		// 衝突していない最後の座標へ戻す
		pos = safePos;

		// 少しだけ法線の方向へ押し出してめり込みを防止する
		pos = VAdd(pos, VScale(hitNormal, SKIN));

		// 床に当たった
		if (hitNormal.y > FLOOR_NORMAL_Y)
		{
			// 下方向の加速度を0にする
			item_->SetVelocityYZero();
		}

		// 衝突後に残っている移動割合
		float remainRatio = (float)(stepCount - hitStep) / stepCount;
		VECTOR remainMove = VScale(move, remainRatio);

		// 壁スライド処理
		// 法線方向成分を除去して壁に沿って移動させる
		float dot = VDot(remainMove, hitNormal);

		if (dot < 0.0f)
		{
			remainMove = VSub(remainMove, VScale(hitNormal, dot));
		}

		// 次の反復で残り移動量を処理
		move = remainMove;
	}

	// 最終位置を反映
	item_->SetPos(VAdd(pos, move));

	// 空中にいるならダメージ処理しない
	if (!isHitStage)
		return;

	// ダメージ計算用の座標を渡す
	item_->SetDamage(pos);
}