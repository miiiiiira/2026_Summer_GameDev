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
				VECTOR capStart = VAdd(nextPos, cap.startOffset);
				VECTOR capEnd = VAdd(nextPos, cap.endOffset);

				// ステージとカプセルの衝突判定
				auto result =
					MV1CollCheck_Capsule(
						stage_->GetModelId(),
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
		if (hitNormal.y < groundNormalY_)
		{
			// 段差判定
			if (CanStepUp(safePos, stepMove, stepHeight_))
			{
				// 衝突していない最後の位置へ戻す
				pos = safePos;

				// 段差の高さ分だけ上へ移動（階段を1段上がるイメージ）
				pos.y += stepHeight_;

				// 今回消費した移動量を残り移動量から除外
				move = VSub(move, stepMove);

				// 次のループで残り移動を処理する
				continue;
			}
		}

		// 衝突していない最後の座標へ戻す
		pos = safePos;

		// 少しだけ法線の方向へ押し出して
		// めり込みを防止する
		pos = VAdd(pos, VScale(hitNormal, skin_));

		// 床判定
		if (hitNormal.y > groundNormalY_)
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
		stage_->GetModelId(),
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

// 小さな段差を登れるか判定する
bool StageCollider::CanStepUp(const VECTOR& pos, const VECTOR& move, float stepHeight)
{
	// テスト用の座標
	VECTOR testPos = pos;

	// 段差の高さ分だけ上へ持ち上げる(階段の1段上に乗れるか確認するため）
	testPos.y += stepHeight;

	// その状態で前方へ移動してみる
	testPos = VAdd(testPos, move);

	bool hit = false;

	for (const auto& cap : capsule_->GetCapsules())
	{
		// 持ち上げた状態でカプセルとステージの衝突判定を行う
		auto result =
			MV1CollCheck_Capsule(
				stage_->GetModelId(),
				-1,
				VAdd(testPos, cap.startOffset),
				VAdd(testPos, cap.endOffset),
				cap.radius);

		// 1つでもポリゴンに当たっていれば衝突
		if (result.HitNum > 0)
		{
			hit = true;
		}

		// 衝突結果のメモリを解放
		MV1CollResultPolyDimTerminate(result);

		if (hit) break;
	}

	// 衝突していなければ段差を登れる
	return !hit;
}
