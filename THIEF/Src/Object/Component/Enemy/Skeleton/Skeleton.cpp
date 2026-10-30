#include "../../../../Scene/SceneManager.h"
#include "../../../../Manager/Audio/AudioManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../PlayerController/PlayerController.h"
#include "../../Transform/Transform.h"
#include "../../Animation/Animation.h"

#include "Skeleton.h"

namespace
{
	// 大きさ
	constexpr VECTOR SCALE = { 0.8f, 0.8f, 0.8f };

	// 敵が反応する座標
	constexpr VECTOR LOOK_POS = { -3700.0f, 10.0f, 1393.0f };

	// 敵がプレイヤーを驚かす座標
	constexpr VECTOR SCARE_POS = { -4000.0f, 10.0f, 1393.0f };

	// 特定のポイントからの反応距離
	constexpr float TRIGGER_RANGE_SQ = 100.0f * 100.0f;

	// アニメーションの再生速度
	constexpr float ANIM_SPEED = 0.3f;

	// 移動スピード
	constexpr float MOVE_SPEED = 20.0f;

	// 予測判定距離
	constexpr float FORWARD_COLLISION_CHECK_DISTANCE = 30.0f;

	// モデルのローカル回転
	constexpr float MODEL_FORWARD_OFFSET_DEG = 180.0f;
}

Skeleton::Skeleton(void)
{
}

Skeleton::~Skeleton(void)
{
}

void Skeleton::InitAnimation(void)
{
	// FBX内のアニメーションをまとめて登録
	AddAnimationAll(static_cast<int>(ANIM_TYPE::MAX), ANIM_SPEED);
}

void Skeleton::InitTransform(void)
{
	// モデルの大きさと正面補正を設定
	ApplyModelTransform(SCALE, MODEL_FORWARD_OFFSET_DEG);
}

void Skeleton::InitPost(void)
{
	// 初期ステート設定
	ChangeState(STATE::IDLE);
}

void Skeleton::Update(void)
{
	// ステートがIDLE以外の場合、遅延回転処理を行う
	if (state_ != STATE::IDLE)
	{
		// 遅延回転処理
		DelayRotate();
	}

	// モデルに回転を反映
	ApplyModelRotation();

	// 現在のステートに応じた更新処理を呼び分ける
	switch (state_)
	{
	case Skeleton::STATE::IDLE:
		UpdateIdle();
		break;

	case Skeleton::STATE::LOOK:
		UpdateLook();
		break;

	case Skeleton::STATE::SCARE:
		UpdateScare();
		break;

	case Skeleton::STATE::END:
		UpdateEnd();
		break;

	default:
		break;
	}

	// アニメーションの更新
	if (animation_)
	{
		animation_->Update();
	}

	// モデルの更新
	MV1RefreshCollInfo(info_.modelId_, -1);
}

void Skeleton::SetSide(ENEMY_SIDE side)
{
	side_ = side;
}

void Skeleton::ChangeState(STATE state)
{
	// 同じ状態への遷移は無視
	if (state_ == state) return;

	state_ = state;

	switch (state_)
	{
	case Skeleton::STATE::IDLE:
		// 待機状態に変更
		ChangeIdle();
		break;

	case Skeleton::STATE::LOOK:
		// 見つめる状態に変更
		ChangeLook();
		break;

	case Skeleton::STATE::SCARE:
		// 怖がらせる状態に変更
		ChangeScare();
		break;

	case Skeleton::STATE::END:
		// 終了状態に変更
		ChangeEnd();
		break;

	default:
		break;
	}
}

void Skeleton::ChangeIdle(void)
{
}

void Skeleton::ChangeLook(void)
{
	// SE再生
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ENEMY_SKELETON_LOOK);
}

void Skeleton::ChangeScare(void)
{
	// 向きを設定
	switch (side_)
	{
	case ENEMY_SIDE::RIGHT:
		info_.moveDir_ = Math::DIR_F;
		break;

	case ENEMY_SIDE::LEFT:
		info_.moveDir_ = Math::DIR_B;
		break;
	}

	// 移動スピードを設定
	info_.moveSpeed_ = MOVE_SPEED;

	// アニメーションの再生
	animation_->Play(static_cast<int>(ANIM_TYPE::ATTACK), false);
}

void Skeleton::ChangeEnd(void)
{
}

void Skeleton::UpdateIdle(void)
{
	// プレイヤーの位置を取得
	VECTOR playerPos = player_->GetTransform()->pos_;

	// プレイヤーとの距離を計算
	float distanceSQ = GetDistanceSQ(LOOK_POS, playerPos);

	// プレイヤーが一定距離以内にいる場合
	if (distanceSQ <= TRIGGER_RANGE_SQ)
	{
		// 見つめる状態に変更
		ChangeState(STATE::LOOK);
		return;
	}
}

void Skeleton::UpdateLook(void)
{
	// プレイヤーの位置を取得
	VECTOR playerPos = player_->GetTransform()->pos_;

	// プレイヤーとの距離を計算
	float distanceSQ = GetDistanceSQ(SCARE_POS, playerPos);

	// プレイヤーが一定距離以内にいる場合
	if (distanceSQ <= TRIGGER_RANGE_SQ)
	{
		// 怖がらせる状態に変更
		ChangeState(STATE::SCARE);
		return;
	}

	// プレイヤーの方向を向く
	LookPlayer();
}

void Skeleton::UpdateScare(void)
{
	// 移動
	Move();

	if (transform_)
	{
		// 進行方向に少し進んだ位置を計算
		VECTOR checkPos = VAdd(
			transform_->pos_,
			VScale(info_.moveDir_,
				FORWARD_COLLISION_CHECK_DISTANCE));

		VECTOR start = VAdd(checkPos, info_.startOffset_);
		VECTOR end = VAdd(checkPos, info_.endOffset_);

		// 壁との衝突チェック
		MV1_COLL_RESULT_POLY_DIM res = MV1CollCheck_Capsule(
			stageId_,
			-1,
			start,
			end,
			info_.radius_);

		if (res.HitNum > 0)
		{
			// 後始末をする
			MV1CollResultPolyDimTerminate(res);

			// SE再生
			AudioManager::GetInstance()->PlaySE(SoundID::SE_ENEMY_SKELETON);

			// 終了状態に変更
			ChangeState(STATE::END);
			return;
		}

		// 後始末をする
		MV1CollResultPolyDimTerminate(res);
	}
}

void Skeleton::UpdateEnd(void)
{
}