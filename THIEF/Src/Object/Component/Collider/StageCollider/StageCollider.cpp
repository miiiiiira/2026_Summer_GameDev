#include "StageCollider.h"
#include <DxLib.h>
#include <algorithm>
#include "../../../Object.h"
#include "../../PlayerController/PlayerController.h"
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

	MATRIX mat = MGetRotY(transform_->angle_.y);

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

	// 衝突とスライドを繰り返す
	for (int bounce = 0; bounce < MAX_BOUNCE; bounce++)
	{
		// 残り移動量の長さ
		float length = VSize(move);

		// ほぼ移動していないなら終了
		if (length < 0.01f)
			break;

		float radius = 1.0f;

		for (const auto& cap : capsule_->GetCapsules())
		{
			radius = max(radius, cap.radius);
		}

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

			bool collision = false;

			VECTOR normalSum = VGet(0.0f, 0.0f, 0.0f);
			float totalWeight = 0.0f;

			for (const auto& cap : capsule_->GetCapsules())
			{
				// カプセル始点・終点を算出
				VECTOR capStart = VAdd(nextPos, VTransform(cap.startOffset, mat));
				VECTOR capEnd = VAdd(nextPos, VTransform(cap.endOffset, mat));

				// ステージとカプセルの衝突判定
				auto result =
					MV1CollCheck_Capsule(
						stage_->GetCollModelId() == -1 ?  stage_->GetModelId(): stage_->GetCollModelId(),
						-1,
						capStart,
						capEnd,
						cap.radius);

				// ヒットしたポリゴンを調べる
				for (int i = 0; i < result.HitNum; i++)
				{
					auto& poly = result.Dim[i];

					// ポリゴン法線
					VECTOR normal = VNorm(poly.Normal);

					// 床の細かい凹凸は無視する
					if (normal.y >= floorNormalY_)
					{
						normal = VGet(0.0f, 1.0f, 0.0f);
					}

					// 現在の移動方向と法線の向きから
					// 正面衝突している度合いを求める
					float push = -VDot(VNorm(move), normal);

					// 背面や平行な面は無視
					if (push <= 0.0f)
						continue;

					normalSum = VAdd(normalSum, VScale(normal, push));
					totalWeight += push;

					collision = true;
				}

				// 衝突結果を解放
				MV1CollResultPolyDimTerminate(result);
			}

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

		// 壁に衝突した場合は段差として登れるか確認する(y成分が小さい法線は壁として扱う)
		if (!isGround_ && hitNormal.y < floorNormalY_)
		{
			// 段差判定
			if (CanStepUp(safePos, stepMove, stepHeight_))
			{
				// 衝突していない最後の位置へ戻す
				pos = safePos;
				pos.y += stepHeight_;

				// 今回消費した移動量を残り移動量から除外
				VECTOR consumed = VSub(pos, prevPos);
				move = VSub(move, consumed);

				// 次のループで残り移動を処理する
				continue;
			}
		}

		// 衝突していない最後の座標へ戻す
		pos = safePos;

		// 少しだけ法線の方向へ押し出して
		// めり込みを防止する
		if (hitNormal.y >= floorNormalY_)
		{
			pos.y += skin_;
		}
		else
		{
			pos = VAdd(pos, VScale(hitNormal, skin_));
		}

		// 床判定
		if (hitNormal.y >= slopeNormalY_)
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
		// 坂の場合
		if (hitNormal.y >= wallNormalY_ &&
			hitNormal.y < floorNormalY_)
		{
			// 坂方向へ移動できるようにする

			VECTOR slopeMove = remainMove;

			// 法線方向の押し込みだけ削除
			float dot = VDot(slopeMove, hitNormal);

			if (dot < 0.0f)
			{
				slopeMove = VSub(
					slopeMove,
					VScale(hitNormal, dot)
				);
			}

			remainMove = slopeMove;
		}
		// 壁の場合
		else
		{
			float dot = VDot(remainMove, hitNormal);

			if (dot < 0.0f)
			{
				remainMove = VSub(
					remainMove,
					VScale(hitNormal, dot)
				);
			}
		}

		// 次の反復で残り移動量を処理
		move = remainMove;
	}

	// 最終位置を反映
	transform_->pos_ = pos;
}

bool StageCollider::CeilingColl(void)
{
	// 必要なコンポーネントが存在しないなら処理しない
	if (!capsule_ || !stage_) return false;

	// 現在座標
	const VECTOR currentPos = transform_->pos_;

	// 立っている場合としゃがんでいる場合のカプセルの始点（頭の座標）
	const VECTOR standingPos = VAdd(currentPos, PlayerController::STANDING_CAP_START_OFFSET);
	const VECTOR crouchingPos = VAdd(currentPos, PlayerController::CROUCHING_CAP_START_OFFSET);

	// 半径を取得
	const float capsuleRadius = capsule_->GetRadius() + RADIUS_OFFSET;

	// 天井に当たっているかどうかのフラグ
	bool isCeiling = false;

	// 念のため、始点と終点が完全に一致している場合は判定をスキップ（安全対策）
	if (standingPos.x == crouchingPos.x && standingPos.y == crouchingPos.y && standingPos.z == crouchingPos.z)
	{
		return false;
	}

	// 立っている場合としゃがんでいる場合の頭座標同士でのカプセルがステージと衝突するかチェック
	MV1_COLL_RESULT_POLY_DIM hitResult = MV1CollCheck_Capsule(
		stage_->GetCollModelId() == -1 ? stage_->GetModelId() : stage_->GetCollModelId(),
		-1,
		standingPos,
		crouchingPos,
		capsuleRadius
	);

	// ヒットしたポリゴン数が0より大きかったら天井がある
	if (hitResult.HitNum > 0)
	{
		isCeiling = true;
	}

	// DXライブラリの衝突結果リソースを解放
	MV1CollResultPolyDimTerminate(hitResult);

	return isCeiling;
}

bool StageCollider::CanStepUp(const VECTOR& pos, const VECTOR& move, float stepHeight)
{
	// 必要なコンポーネントが存在しない場合は判定不可
	if (!capsule_ || !stage_)
		return false;

	// カプセルのオフセットをワールド座標へ変換するために使用
	MATRIX mat = MGetRotY(transform_->angle_.y);

	// 一旦持ち上げる
	VECTOR testPos = pos;
	testPos.y += stepHeight;

	// 前進を少しずつ試す
	const int FORWARD_STEP = 8;

	for (int s = 1; s <= FORWARD_STEP; s++)
	{
		float rate = (float)s / FORWARD_STEP;

		VECTOR checkPos = testPos;
		checkPos = VAdd(checkPos, VScale(move, rate));

		bool hitWall = false;

		for (const auto& cap : capsule_->GetCapsules())
		{
			VECTOR start = VAdd(checkPos, VTransform(cap.startOffset, mat));
			VECTOR end = VAdd(checkPos, VTransform(cap.endOffset, mat));

			auto result = MV1CollCheck_Capsule(
				stage_->GetCollModelId() == -1 ? stage_->GetModelId() : stage_->GetCollModelId(),
				-1,
				start,
				end,
				cap.radius);

			for (int i = 0; i < result.HitNum; i++)
			{
				VECTOR normal = VNorm(result.Dim[i].Normal);

				// 壁だけを見る
				if (normal.y <= wallNormalY_)
				{
					hitWall = true;
					break;
				}
			}

			MV1CollResultPolyDimTerminate(result);

			if (hitWall)
				break;
		}

		// 少しでも前へ進めないなら失敗
		if (hitWall)
			return false;
	}

	// 最後まで壁が無ければ
	testPos = VAdd(testPos, move);

	// 下へ少しずつ落として床を探す
	const float DROP_STEP = 1.0f;

	// 現在どれだけ下降したか
	float dropped = 0.0f;

	while (dropped <= stepHeight)
	{
		// 落下後の判定位置
		VECTOR dropPos = testPos;

		// 少しずつ下げる
		dropPos.y -= dropped;

		// 全カプセルで床判定
		for (const auto& cap : capsule_->GetCapsules())
		{
			// カプセルをワールド座標へ変換
			VECTOR start = VAdd(dropPos, VTransform(cap.startOffset, mat));
			VECTOR end = VAdd(dropPos, VTransform(cap.endOffset, mat));

			// ステージとの衝突確認
			auto result = MV1CollCheck_Capsule(
				stage_->GetCollModelId() == -1 ? stage_->GetModelId() : stage_->GetCollModelId(),
				-1,
				start,
				end,
				cap.radius);

			// ヒットした面が床か確認
			for (int i = 0; i < result.HitNum; i++)
			{
				// ポリゴン法線
				VECTOR normal = VNorm(result.Dim[i].Normal);

				// Y方向の法線が大きければ床
				if (normal.y >= floorNormalY_)
				{
					MV1CollResultPolyDimTerminate(result);
					return true;
				}
			}

			// 衝突結果解放
			MV1CollResultPolyDimTerminate(result);
		}

		// 次の高さを調べる
		dropped += DROP_STEP;
	}

	// どの高さでも床を見つけられなかった
	return false;
}
