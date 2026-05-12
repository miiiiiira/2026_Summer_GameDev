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

	// 地面判定
	FieldCollision();

	// 壁判定
	WallCollision();
}

// 地面判定
void StageCollider::FieldCollision()
{
	// モデルID取得
	int model = stage_->GetModelId();

	// 座標を取得
	VECTOR pos = transform_->pos_;

	// ローカル→ワールド座標に変換
	VECTOR worldPos = stage_->ToWorldPos(transform_->pos_);

	// 上から下へラインを飛ばす(レイ)
	VECTOR start = worldPos;
	start.y += 10.0f;
	VECTOR end = worldPos;
	end.y -= 200.0f;

	// ライン衝突をチェック
	auto res = MV1CollCheck_Line(model, -1, start, end);

	// 衝突していないなら処理しない
	if (!res.HitFlag) return;

	// 地面の法線を取得
	VECTOR normal = res.Normal;

	// 法線と上方向の角度
	float dot = VDot(normal, VGet(0.0f, 1.0f, 0.0f));
	float angle = acosf(dot) * (180.0f / DX_PI_F);

	// 衝突地点Y
	float hitY = stage_->ToLocalPos(res.HitPosition).y;

	// 高さ差
	float diff = hitY - transform_->pos_.y;

	// 登れる段差
	const float STEP_HEIGHT = 30.0f;

	// 接地許容
	const float GROUND_EPSILON = 2.0f;

	// 登れる坂か？
	if (angle <= slopeLimit_)
	{
		// 段差内
		if (diff > -STEP_HEIGHT)
		{
			// 地面高さへ補正
			pos.y += diff;

			// 接地判定
			if (diff >= -GROUND_EPSILON)
			{
				isGround_ = true;
			}
		}
	}

	transform_->pos_ = pos;
}
// 壁判定
void StageCollider::WallCollision()
{
	if (!capsule_) return;

	if (!stage_) return;

	// モデルIDを取得
	int model = stage_->GetModelId();

	// 座標を取得
	VECTOR pos = transform_->pos_;

	// カプセル取得
	VECTOR capStart = capsule_->GetStart();
	VECTOR capEnd = capsule_->GetEnd();

	// カプセルとモデルの衝突チェック
	auto hits = MV1CollCheck_Capsule(
		model,
		-1,
		capStart,
		capEnd,
		capsule_->radius_
	);

	// 全当たり判定処理
	for (int i = 0; i < hits.HitNum; i++)
	{
		auto hit = hits.Dim[i];
		// 最大5回押し戻し
		for (int tryCnt = 0; tryCnt < 5; tryCnt++)
		{

			// まだめり込んでいるか？
			if (!HitCheck_Capsule_Triangle(
				capStart,
				capEnd,
				capsule_->radius_,
				hit.Position[0],
				hit.Position[1],
				hit.Position[2]
			))
			{
				break;
			}

			// ポリゴンの法線
			VECTOR normal = hit.Normal;

			// Yは無視する
			normal.y = 0.0f;

			// 長さチェック
			if (VSize(normal) < 0.001f) break;

			// 正規化
			normal = VNorm(normal);

			// 押し戻し量
			const float PUSH_POWER = 2.0f;
			VECTOR push = VScale(normal, PUSH_POWER);

			// 押し戻し
			pos = VAdd(pos, push);

			// 押し戻したのでカプセルの位置も更新
			capStart = VAdd(capStart, push);
			capEnd = VAdd(capEnd, push);
		}
	}

	// 衝突情報を削除する
	MV1CollResultPolyDimTerminate(hits);
	transform_->pos_ = pos;
}
// 通常移動
VECTOR StageCollider::ResolveMove(VECTOR move)
{

	if (!capsule_ || !stage_) return move;
	int model = stage_->GetModelId();
	VECTOR pos = transform_->pos_;

	// 移動後位置
	VECTOR nextPos = VAdd(pos, move);

	// 仮カプセル
	VECTOR capStart = VAdd(nextPos, capsule_->startOffset_);
	VECTOR capEnd = VAdd(nextPos, capsule_->endOffset_);

	// 衝突判定
	auto hits = MV1CollCheck_Capsule(
		model,
		-1,
		capStart,
		capEnd,
		capsule_->radius_
	);

	// 全ヒット処理
	for (int i = 0; i < hits.HitNum; i++)
	{
		auto& hit = hits.Dim[i];

		// 法線
		VECTOR normal = hit.Normal;

		// 傾斜角取得
		float dotUp = VDot(normal, VGet(0, 1, 0));

		// 安全clamp
		dotUp = std::clamp(dotUp, -1.0f, 1.0f);
		float angle = acosf(dotUp) * (180.0f / DX_PI_F);

		// 急斜面
		if (angle > slopeLimit_)
		{
			// 水平方向のみ
			normal.y = 0.0f;
			float len = VSize(normal);
			if (len < 0.001f) continue;

			// 正規化
			normal = VNorm(normal);

			// 壁方向成分
			float dot = VDot(move, normal);

			// 壁へ進んでいる場合
			if (dot < 0.0f)
			{
				// 壁方向成分除去
				move = VSub(move, VScale(normal, dot));
			}
		}
	}

	// 衝突情報を削除する
	MV1CollResultPolyDimTerminate(hits);
	return move;
}
// 段差対応移動
VECTOR StageCollider::ResolveStepMove(VECTOR move)
{
	if (!capsule_ || !stage_) return move;
	int model = stage_->GetModelId();
	VECTOR pos = transform_->pos_;

	// 通常移動位置
	VECTOR nextPos = VAdd(pos, move);
	VECTOR capStart = VAdd(nextPos, capsule_->startOffset_);
	VECTOR capEnd = VAdd(nextPos, capsule_->endOffset_);

	// 衝突チェック
	auto hit = MV1CollCheck_Capsule(
		model, -1,
		capStart, capEnd,
		capsule_->radius_
	);

	// 当たらなければそのまま
	if (hit.HitNum == 0)
	{
		// 衝突情報を削除する
		MV1CollResultPolyDimTerminate(hit);
		return move;
	}

	// 段差高さ
	const float STEP_HEIGHT = 15.0f;

	// 少し持ち上げる
	VECTOR stepPos = pos;
	stepPos.y += STEP_HEIGHT;
	VECTOR stepNext = VAdd(stepPos, move);
	VECTOR stepStart = VAdd(stepNext, capsule_->startOffset_);
	VECTOR stepEnd = VAdd(stepNext, capsule_->endOffset_);

	// 再判定
	auto stepHit = MV1CollCheck_Capsule(
		model, -1,
		stepStart, stepEnd,
		capsule_->radius_
	);

	// 登れる
	if (stepHit.HitNum == 0)
	{
		move.y += STEP_HEIGHT;
		return move;
	}

	// 衝突情報を削除する
	MV1CollResultPolyDimTerminate(hit);

	// ダメならスライド
	return ResolveMove(move);
}