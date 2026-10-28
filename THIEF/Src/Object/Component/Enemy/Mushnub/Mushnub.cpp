#include "../../../../Scene/SceneManager.h"
#include "../../../../Manager/Audio/AudioManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../PlayerController/PlayerController.h"
#include "../../Collider/StageCollider/StageCollider.h"
#include "../../Animation/Animation.h"
#include "../../Transform/Transform.h"

#include "Mushnub.h"

namespace
{
	// 大きさ
	constexpr VECTOR SCALE = { 0.3f, 0.3f, 0.3f };

	// 攻撃時ダメージ力
	constexpr float ATTACK_DAMAGE_POWER = 10.0f;

	// 待機時間
	constexpr float STEP_TIME_IDLE = 5.0f;

	// 発見時の驚き演出時間
	constexpr float STEP_TIME_SURPRISE = 2.0f;

	// 追跡移動速度
	constexpr float SPEED_CHASE = 3.0f;

	// 元の場所へ戻り切ったと判定する閾値
	constexpr float RETURN_THRESHOLD_DISTANCE_SQ = 100.0f * 100.0f;

	// プレイヤーへの距離を保つための閾値
	constexpr float PLAYER_KEEP_DISTANCE_SQ = 200.0f * 200.0f;

	// アニメーションの再生速度
	constexpr float ANIM_SPEED = 0.2f;

	// モデルのローカル回転
	constexpr float MODEL_FORWARD_OFFSET_DEG = 180.0f;
}

Mushnub::Mushnub(void)
{
}

Mushnub::~Mushnub(void)
{
}

void Mushnub::InitParam(void)
{
	// 攻撃時ダメージ力
	info_.attackDamagePow_ = ATTACK_DAMAGE_POWER;
}

void Mushnub::InitAnimation(void)
{
	// FBX内のアニメーションをまとめて登録
	AddAnimationAll(static_cast<int>(ANIM_TYPE::MAX), ANIM_SPEED);
}

void Mushnub::InitTransform(void)
{
	// モデルの大きさと正面補正を設定
	ApplyModelTransform(SCALE, MODEL_FORWARD_OFFSET_DEG);
}

void Mushnub::InitPost(void)
{
	// 初期ステート設定
	ChangeState(STATE::IDLE);
}

void Mushnub::Update(void)
{
	// 遅延回転処理
	DelayRotate();

	// モデルに回転を反映
	ApplyModelRotation();

	// 現在のステートに応じた更新処理を呼び分ける
	switch (state_)
	{
	case STATE::IDLE:
		UpdateIdle();
		break;

	case STATE::SURPRISE:
		UpdateSurprise();
		break;

	case STATE::CHASE:
		UpdateChase();
		break;

	default:
		break;
	}

	// 重力処理
	ApplyGravity();

	// ステージとの衝突判定・押し出し計算
	if (stageColl_)
	{
		stageColl_->StageColl(info_.velocityY_);
	}

	// アニメーションの更新
	if (animation_)
	{
		animation_->Update();
	}

	// モデルの更新
	MV1RefreshCollInfo(info_.modelId_, -1);
}

void Mushnub::Draw3D(void)
{
	EnemyBase::Draw3D();

#ifdef _DEBUG
	// プレイヤーを検知する行動エリアをマゼンタのワイヤーフレームで描画
	DrawCube3D(minAreaPos_, maxAreaPos_, 0xff00ff, 0xff00ff, false);
#endif
}

void Mushnub::SetAreaPos(VECTOR minPos, VECTOR maxPos)
{
	minAreaPos_ = minPos;
	maxAreaPos_ = maxPos;
}

void Mushnub::SetHomePos(VECTOR pos)
{
	homePos_ = pos;
}

void Mushnub::ChangeState(STATE state)
{
	// 同じ状態への遷移は無視
	if (state_ == state) return;

	state_ = state;

	// 遷移時に一度だけ行う初期化処理を呼び出す
	switch (state_)
	{
	case STATE::IDLE:
		ChangeIdle();
		break;

	case STATE::SURPRISE:
		ChangeSurprise();
		break;

	case STATE::CHASE:
		ChangeChase();
		break;

	default:
		break;
	}
}

void Mushnub::ChangeIdle(void)
{
	// 待機する時間を設定
	info_.stateTimer_ = STEP_TIME_IDLE;

	// 待機アニメーションをループ再生
	animation_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void Mushnub::ChangeSurprise(void)
{
	// 驚いている時間（猶予時間）を設定
	info_.stateTimer_ = STEP_TIME_SURPRISE;

	// プレイヤーの方を向く
	LookPlayer();

	// 発見時の鳴き声SEを再生
	AudioManager::GetInstance()->PlaySE(
					SoundID::SE_ENEMY_MUSHNUB,
					&transform_->pos_);

	// 驚きのアニメーションを1回だけ再生
	animation_->Play(static_cast<int>(ANIM_TYPE::HIT_REACT), false);
}

void Mushnub::ChangeChase(void)
{
	// 追跡時の移動速度
	info_.moveSpeed_ = SPEED_CHASE;

	// 歩きアニメーションをループ再生
	animation_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
}

void Mushnub::UpdateIdle(void)
{
	// プレイヤーが行動エリア内に入ったら発見状態へ
	if (IsPlayerInArea(minAreaPos_, maxAreaPos_))
	{
		ChangeState(STATE::SURPRISE);
		return;
	}
}

void Mushnub::UpdateSurprise(void)
{
	// 驚き時間を経過時間分減らす
	info_.stateTimer_ -= SceneManager::GetInstance()->GetDeltaTime();

	if (info_.stateTimer_ < 0.0f)
	{
		// 驚き終了
		ChangeState(STATE::CHASE);
		return;
	}
}

void Mushnub::UpdateChase(void)
{
	if (IsPlayerInArea(minAreaPos_, maxAreaPos_))
	{
		// プレイヤーがエリア内にいる間は、プレイヤーの方を向き続ける
		LookPlayer();
	}
	else
	{
		// プレイヤーがエリア外に出たら、帰還先へ向かう
		// 帰還先へのベクトルを計算
		VECTOR diff = VSub(homePos_, transform_->pos_);

		// 高さは無視して水平方向のみで移動させる
		diff.y = 0.0f;

		// ベクトルの正規化で単位ベクトルを取得
		info_.moveDir_ = VNorm(diff);

		// 回転はY軸のみ
		transform_->angle_.x = transform_->angle_.z = 0.0f;

		// 帰還先までの距離を計算
		float distanceToHomePosSQ = GetDistanceSQ(transform_->pos_, homePos_);

		// 帰還先に十分近づいたら待機状態に戻る
		if (distanceToHomePosSQ <= RETURN_THRESHOLD_DISTANCE_SQ)
		{
			ChangeState(STATE::IDLE);
			return;
		}
	}

	// プレイヤーとの距離を計算
	float distToPlayerSQ = GetDistanceSQ(transform_->pos_, player_->GetTransform()->pos_);

	// プレイヤーとの距離が保つべき距離以上なら、歩いて近づく
	if (distToPlayerSQ >= PLAYER_KEEP_DISTANCE_SQ)
	{
		animation_->Play(static_cast<int>(ANIM_TYPE::WALK), true);

		Move();
	}
	else
	{
		// 十分近い場合は、その場で立ち止まって待機アニメーションにする
		animation_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
	}
}