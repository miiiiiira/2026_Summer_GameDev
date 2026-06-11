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
}

// ステージとの衝突判定
// VelocityY は床や天井に衝突した際に補正するため参照渡し
void StageCollider::StageColl(float& velocityY)
{
	// 必要なコンポーネントが存在しないなら処理しない
	if (!capsule_) return;
	if (!stage_) return;

	// 現在座標
	VECTOR currentPos = transform_->pos_;

	// 前フレーム座標
	VECTOR prevPos = transform_->prevPos_;

	// 今フレーム移動量
	VECTOR move = VSub(currentPos, prevPos);

	// 衝突判定開始座標
	VECTOR pos = prevPos;

	// 毎フレーム初期化
	isGround_ = false;

	// 1フレーム中に複数回衝突する場合に対応するための最大反復回数
	const int MAX_BOUNCE = 5;

	// めり込み防止用の押し出し量
	const float SKIN = 0.01f;

	// 衝突とスライドを繰り返す
	for (int bounce = 0; bounce < MAX_BOUNCE; bounce++)
	{
		// 残り移動量の長さ
		float length = VSize(move);

		// ほぼ移動していないなら終了
		if (length < 0.01f)
			break;

		float radius = capsule_->radius_;

		// 高速移動時のすり抜け防止のため、
		// 移動経路を細かく分割して判定する
		int stepCount = (int)(length / (radius * 0.1f)) + 1;

		// 分割数の上限・下限を設定
		stepCount = std::clamp(stepCount, 1, 64);

		// 1ステップ当たりの移動量
		VECTOR stepMove = VScale(move, 1.0f / stepCount);

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

			// カプセル始点・終点を算出
			VECTOR capStart = VAdd(nextPos, capsule_->startOffset_);
			VECTOR capEnd = VAdd(nextPos, capsule_->endOffset_);

			// ステージとカプセルの衝突判定
			auto result =
				MV1CollCheck_Capsule(
					stage_->GetModelId(),
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

				// 現在の移動方向と法線の向きから
				// 正面衝突している度合いを求める
				float push = -VDot(VNorm(move), normal);

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

		// 衝突していない最後の座標へ戻す
		pos = safePos;

		// 少しだけ法線の方向へ押し出して
		// めり込みを防止する
		pos = VAdd(pos, VScale(hitNormal, SKIN));

		// 床判定
		if (hitNormal.y > 0.6f)
		{
			// 接地フラグを立てる
			isGround_ = true;

			// 落下速度を停止
			if (velocityY < 0.0f)
			{
				velocityY = 0.0f;
			}
		}

		// 天井判定
		if (hitNormal.y < -0.6f)
		{
			// 上昇速度を停止
			if (velocityY > 0.0f)
			{
				velocityY = 0.0f;
			}
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
	transform_->pos_ = pos;
}
