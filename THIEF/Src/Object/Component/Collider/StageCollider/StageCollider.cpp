#include "StageCollider.h"
#include <DxLib.h>
#include <algorithm>
#include "../../../Object.h"
#include "../../Stage/Stage.h"

// 初期化
void StageCollider::Init()
{
	// Transfrom取得
	transform_ = owner_->GetComponent<Transform>();

	// CapsuleCollider取得
	capsule_ = owner_->GetComponent<CapsuleCollider>();

	// ステージTransform取得
	if (stage_)
	{
		stageTransform_ = stage_->GetTransform();
	}
}

void StageCollider::Update()
{
	if (!transform_) return;

	if (!capsule_) return;

	if (!stage_) return;

	StageCollision();

	//// 地面判定
	//FieldCollision();

	//// 壁判定
	//WallCollision();
}

// 地面判定
void StageCollider::StageCollision()
{
	// 現在の座標
	VECTOR currentPos = transform_->pos_;

	// 前回の座標取得
	VECTOR prevPos = transform_->prevPos_;

	// 移動量(前回の座標と現在の座標の差分)
	VECTOR move = VSub(currentPos, prevPos);

	// 移動距離(距離を取る)
	float distance = VSize(move);

	// プレイヤーの半径取得
	float playerRad = capsule_->radius_;

	// 半径ベースで分割
	int stepCount = (int)(distance / playerRad) + 1;

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
		VECTOR capStart = VAdd(capsule_->GetStart(), stepMove);

		// カプセル終了座標
		VECTOR capEnd = VAdd(capsule_->GetEnd(), stepMove);

		// 衝突判定　ステージモデルとカプセル(アイテム)
		MV1_COLL_RESULT_POLY_DIM hitResult =
			MV1CollCheck_Capsule(
				stage_->GetModelId(),
				-1,
				capStart,
				capEnd,
				playerRad
			);

		// 当たっていなければ移動確定
		if (hitResult.HitNum <= 0)
		{
			testPos = nextPos;

			// 衝突情報の解放
			MV1CollResultPolyDimTerminate(hitResult);

			// 当たっているためフラグをたてる
			isGround_ = true;

			continue;
		}

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
					playerRad,
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
	transform_->pos_ = testPos;

}

//// 地面判定
//void StageCollider::FieldCollision()
//{
//	// モデルID取得
//	int model = stage_->GetModelId();
//
//	// 座標を取得
//	VECTOR pos = transform_->pos_;
//
//	// ローカル→ワールド座標に変換
//	VECTOR worldPos = stage_->ToWorldPos(transform_->pos_);
//
//	// 上から下へラインを飛ばす(レイ)
//	VECTOR start = worldPos;
//	start.y += 50.0f;
//
//	VECTOR end = worldPos;
//	end.y -= 0.0f;
//
//	// ライン衝突をチェック
//	auto res = MV1CollCheck_Line(model, -1, start, end);
//
//	// 衝突していないなら処理しない
//	if (!res.HitFlag) return;
//
//	// 地面の法線を取得
//	VECTOR normal = res.Normal;
//
//	// 法線と上方向の角度
//	float dot = VDot(normal, VGet(0.0f, 1.0f, 0.0f));
//	float angle = acosf(dot) * (180.0f / DX_PI_F);
//
//	// 衝突地点Y
//	float hitY = stage_->ToLocalPos(res.HitPosition).y;
//
//	// 高さ差
//	float diff = hitY - transform_->pos_.y;
//
//	// 登れる段差
//	const float STEP_HEIGHT = 30.0f;
//
//	// 接地許容
//	const float GROUND_EPSILON = 2.0f;
//
//	// 登れる坂か？
//	if (angle <= slopeLimit_)
//	{
//		// 段差内
//		if (diff > -STEP_HEIGHT)
//		{
//			// 地面高さへ補正
//			pos.y += diff;
//
//			// 接地判定
//			if (diff >= -GROUND_EPSILON)
//			{
//				isGround_ = true;
//			}
//		}
//	}
//
//	transform_->pos_ = pos;
//}
//// 壁判定
//void StageCollider::WallCollision()
//{
//	if (!capsule_) return;
//
//	if (!stage_) return;
//
//	// モデルIDを取得
//	int model = stage_->GetModelId();
//
//	// 座標を取得
//	VECTOR pos = transform_->pos_;
//
//	// カプセル取得
//	VECTOR capStart = capsule_->GetStart();
//	VECTOR capEnd = capsule_->GetEnd();
//
//	// カプセルとモデルの衝突チェック
//	auto hits = MV1CollCheck_Capsule(
//		model,
//		-1,
//		capStart,
//		capEnd,
//		capsule_->radius_
//	);
//
//	// 全当たり判定処理
//	for (int i = 0; i < hits.HitNum; i++)
//	{
//		auto hit = hits.Dim[i];
//		// 最大5回押し戻し
//		for (int tryCnt = 0; tryCnt < 5; tryCnt++)
//		{
//
//			// まだめり込んでいるか？
//			if (!HitCheck_Capsule_Triangle(
//				capStart,
//				capEnd,
//				capsule_->radius_,
//				hit.Position[0],
//				hit.Position[1],
//				hit.Position[2]
//			))
//			{
//				break;
//			}
//
//			// ポリゴンの法線
//			VECTOR normal = hit.Normal;
//
//			// Yは無視する
//			normal.y = 0.0f;
//
//			// 長さチェック
//			if (VSize(normal) < 0.001f) break;
//
//			// 正規化
//			normal = VNorm(normal);
//
//			// 押し戻し量
//			const float PUSH_POWER = 2.0f;
//			VECTOR push = VScale(normal, PUSH_POWER);
//
//			// 押し戻し
//			pos = VAdd(pos, push);
//
//			// 押し戻したのでカプセルの位置も更新
//			capStart = VAdd(capStart, push);
//			capEnd = VAdd(capEnd, push);
//		}
//	}
//
//	// 衝突情報を削除する
//	MV1CollResultPolyDimTerminate(hits);
//	transform_->pos_ = pos;
//}
