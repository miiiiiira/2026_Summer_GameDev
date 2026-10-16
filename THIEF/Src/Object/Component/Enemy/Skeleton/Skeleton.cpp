#include "../../../../Scene/SceneManager.h"
#include "../../../../Manager/Audio/AudioManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../../../Common/Transform/MatrixUtility.h"
#include "../../PlayerController/PlayerController.h"
#include "../../Collider/3DCollider/CapsuleCollider.h"
#include "../../Collider/StageCollider/StageCollider.h"
#include "../../Transform/Transform.h"
#include "../../Animation/Animation.h"
#include "../EnemyCommon.h"

#include "Skeleton.h"

namespace
{
	// 大きさ
	constexpr VECTOR SCALE = { 0.8f,0.8f,0.8f };

	// 向き
	constexpr VECTOR DEFAULT_ANGLE = { 0.0f, 0.0f,0.0f };

	// 敵が反応する座標
	constexpr VECTOR LOOK_POS = { -3700.0f, 10.0f, 1393.0f };

	// 敵がプレイヤーを驚かす座標
	constexpr VECTOR SCARE_POS = { -4000.0f, 10.0f, 1393.0f };

	// 特定のポイントからの反応距離
	constexpr float TRIGGER_RANGE = 100.0f * 100.0f;

	// アニメーションの再生速度
	constexpr float ANIM_SPEED = 0.3f;

	// 移動スピード
	constexpr float MOVE_SPEED = 20.0f;

	// 予測判定距離
	constexpr float FORWARD_COLLISION_CHECK_DISTANCE = 30.0f;
}

Skeleton::Skeleton(void)
{
}

Skeleton::~Skeleton(void)
{
}

void Skeleton::Init(void)
{
	EnemyBase::Init();

	// アニメーションの初期化
	animation_->Init();

	// アニメーションの追加
	for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
	{
		animation_->AddInFbx(
					static_cast<int>(i), 
					ANIM_SPEED,
					static_cast<int>(i));
	}

	// 敵のデータを取得して設定
	const auto& data = EnemyTable::Table.at(ENEMY_TAG::SKELETON);
	SetEnemyData(data);

	// パラメータ初期化
	info_.moveDir_ = Math::VECTOR_ZERO;

	// 敵のタグを設定
	info_.tag_ = ENEMY_TAG::SKELETON;

	if (transform_)
	{
		// モデルの大きさを設定
		info_.scale_ = SCALE;
		MV1SetScale(info_.modelId_, info_.scale_);

		// 敵の向きを設定
		transform_->angle_ = DEFAULT_ANGLE;
		info_.localAngle_ = { 0.0f, Math::Deg2Rad(180.0f), 0.0f };

		// 前回の座標を現在の座標に設定
		transform_->prevPos_ = transform_->pos_;
	}

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

	// モデルが存在する場合、回転を設定
	if (info_.modelId_ != -1)
	{
		// 敵の現在の向き（遅延回転などで計算した角度）＋ ローカル回転補正
		VECTOR finalAngle;
		finalAngle.x = transform_->angle_.x + info_.localAngle_.x;
		finalAngle.y = transform_->angle_.y + info_.localAngle_.y;
		finalAngle.z = transform_->angle_.z + info_.localAngle_.z;

		// モデルに回転をセット
		MV1SetRotationXYZ(info_.modelId_, finalAngle);
	}

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

void Skeleton::Draw3D(void)
{
	EnemyBase::Draw3D();
}

void Skeleton::SetSide(ENEMY_SIDE side)
{
	side_ = side;
}

void Skeleton::ChangeState(STATE state)
{
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
		info_.moveDir_ = { 0.0f, 0.0f, 1.0f };
		break;

	case ENEMY_SIDE::LEFT:
		info_.moveDir_ = { 0.0f, 0.0f, -1.0f };
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
	float distance = GetDistanceSQ(LOOK_POS, playerPos);

	// プレイヤーが一定距離以内にいる場合
	if (distance <= TRIGGER_RANGE)
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
	float distance = GetDistanceSQ(SCARE_POS, playerPos);

	// プレイヤーが一定距離以内にいる場合
	if (distance <= TRIGGER_RANGE)
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