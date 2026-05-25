#include "ItemCollider.h"
#include "../../../Object.h"
#include "../../PlayerController/PlayerController.h"
#include "../../Stage/Stage.h"
#include "../../Item/Item.h"
#include "../../../../Common/Transform/MatrixUtility.h"

void ItemCollider::Init(void)
{
	// アイテムの確保
	item_ = owner_->GetComponent<Item>();
}

void ItemCollider::Update(void)
{

	if (!item_) return;

	if (!player_) return;

	if (!stage_) return;

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
	VECTOR topPos = player_->GetLineStartPos();

	// 線分の下座標
	VECTOR downPos = player_->GetLineEndPos();

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
	// 接触したかどうか
	bool isHitStage = false;

	// 現在の座標
	VECTOR currentPos = item_->GetTransform()->pos_;

	// 前回の座標取得
	VECTOR prevPos = item_->GetInfo().prevPos_;

	// 移動量(前回の座標と現在の座標の差分)
	VECTOR move = VSub(currentPos, prevPos);

	// 移動距離(距離を取る)
	float distance = VSize(move);

	// アイテムの半径取得
	float itemRad = item_->GetInfo().collisionRadiusX_;

	// 半径ベースで分割
	int stepCount = (int)(distance / itemRad) + 1;

	// 最大ステップ数
	const int MAX_STEP = 16;

	// 最低制限(最低でも1回は動くようにする)
	if (stepCount < 1)
		stepCount = 1;

	// 最大制限(試行回数が多すぎて重くなるのを防ぐ)
	if (stepCount > MAX_STEP)
		stepCount = MAX_STEP;

	// 1ステップ移動量を計算
	VECTOR stepMove = VScale(move, 1.0f / stepCount);

	// 判定用座標
	VECTOR testPos = prevPos;

	// 分割移動
	for (int step = 0; step < stepCount; step++)
	{
		// 判定用座標に1ステップの移動量を足し、次座標を計算
		VECTOR nextPos = VAdd(testPos, stepMove);

		// カプセル開始座標
		VECTOR capStart = nextPos;
		capStart.y -= item_->GetInfo().collisionRadiusY_;

		// カプセル終了座標
		VECTOR capEnd = nextPos;
		capEnd.y += item_->GetInfo().collisionRadiusY_;

		// 衝突判定　ステージモデルとカプセル(アイテム)
		MV1_COLL_RESULT_POLY_DIM hitResult =
			MV1CollCheck_Capsule(
				stage_->GetModelId(),
				-1,
				capStart,
				capEnd,
				itemRad
			);

		// 当たっていなければ移動確定
		if (hitResult.HitNum <= 0)
		{
			testPos = nextPos;

			// 衝突情報の解放
			MV1CollResultPolyDimTerminate(hitResult);

			continue;
		}

		// 当たっているためフラグをたてる
		isHitStage = true;

		// 衝突押し戻し
		for (int i = 0; i < hitResult.HitNum; i++)
		{
			auto& hit = hitResult.Dim[i];

			// 法線
			VECTOR normal = hit.Normal;

			// 法線の長さを確認
			float normalLength = VSize(normal);

			// 小さすぎたら処理をしない
			if (normalLength < 0.0001f)
				continue;

			// 正規化
			normal = VNorm(normal);

			// めり込み解消

			// 三角形最近点取得
			bool isHit =
				HitCheck_Capsule_Triangle(
					capStart,
					capEnd,
					itemRad,
					hit.Position[0],
					hit.Position[1],
					hit.Position[2]
				);

			// めり込んでなければスキップ
			if (!isHit)
				continue;

			// 押し戻し量計算

			// 少しずつ押し戻す
			const float PUSH_BACK = 0.5f;

			// 押し出し量を計算
			VECTOR push = VScale(normal, PUSH_BACK);

			// 押し戻し
			nextPos = VAdd(nextPos, push);

			// 壁沿い移動
			float dot = VDot(stepMove, normal);

			// 壁に向かっている場合だけ
			if (dot < 0.0f)
			{
				// 法線成分除去
				VECTOR slide =
					VSub(
						stepMove,
						VScale(normal, dot)
					);

				// スライド移動へ変更
				stepMove = slide;
			}

			// カプセル位置更新
			capStart = VAdd(capStart, push);
			capEnd = VAdd(capEnd, push);
		}

		// 衝突情報の解放
		MV1CollResultPolyDimTerminate(hitResult);
	}

	// 最終位置更新
	item_->SetPos(testPos);

	// 空中にいるならダメージ処理しない
	if (!isHitStage)return;

	// 納品場所にはいっているなら処理をしない
	if (item_->GetInfo().hasTouchedDeliveryLocation_)return;

	//　持っていても空中判定になっていなければ、処理をしない
	if (item_->GetInfo().isGrabbed && item_->GetInfo().hasTouchedStage_)return;

	float hitSpeed = 0;

	// アイテムが掴まれていたら
	if (item_->GetInfo().isGrabbed)
	{
		hitSpeed = VSize(VSub(currentPos, prevPos));
		// 重力分を引いておく(重力でお金が削れるのを防ぐため)
		hitSpeed -= VSize(item_->GetInfo().velocity_);
	}
	// 掴まれていないかつ、空中状態から1度も設置していなかったら
	else if (!item_->GetInfo().isGrabbed && !item_->GetInfo().hasTouchedStage_)
	{
		hitSpeed = VSize(VSub(item_->GetInfo().grabbedPos_, testPos));
		// 重力分を引いておく(重力でお金が削れるのを防ぐため)
		hitSpeed -= VSize(item_->GetInfo().velocity_);

		// 設置したためフラグを接触フラグを立てる
		item_->TrueHasTouchedStage();
	}

	// スピードをそのままダメージに変換
	int damage = static_cast<int>(hitSpeed);

	// マイナス値になるのを防ぐ
	item_->SetDamage(abs(damage));
}
