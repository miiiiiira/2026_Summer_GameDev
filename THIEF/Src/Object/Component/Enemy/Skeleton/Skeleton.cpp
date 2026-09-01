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

Skeleton::Skeleton(void)
{
}

Skeleton::~Skeleton(void)
{
}

void Skeleton::Init(void)
{
	EnemyBase::Init();

	anim_->Init();
	for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
	{
		anim_->AddInFbx(static_cast<int>(i), 0.3f, static_cast<int>(i));
	}

	const auto& data = EnemyTable::Table.at(ENEMY_TAG::SKELETON);
	SetEnemyData(data);

	// パラメータ初期化
	info_.moveDir_ = Math::VECTOR_ZERO;
	info_.tag_ = ENEMY_TAG::SKELETON;


	if (transform_)
	{
		info_.scale_ = SCALE;
		MV1SetScale(info_.modelId_, info_.scale_);


		transform_->angle_ = DEFAULT_ANGLE;
		info_.localAngle_ = { 0.0f, Math::Deg2Rad(180.0f), 0.0f };

		MATRIX mat = Matrix::Multiplication(info_.localAngle_, transform_->angle_);

		transform_->prevPos_ = transform_->pos_;
	}

	// 初期ステート設定
	ChangeState(STATE::IDLE);
}

void Skeleton::Update(void)
{
	if (state_ != STATE::IDLE)
	{
		// 遅延回転処理
		DelayRotate();
	}

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


	switch (state_)
	{
	case Skeleton::STATE::IDLE: UpdateIdle(); break;
	case Skeleton::STATE::LOOK: UpdateLook(); break;
	case Skeleton::STATE::SCARE: UpdateScare(); break;
	case Skeleton::STATE::END: UpdateEnd(); break;
	default:
		break;
	}

	// アニメーションの更新
	if (anim_)
	{
		anim_->Update();
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
	case Skeleton::STATE::IDLE: ChangeIdle(); break;
	case Skeleton::STATE::LOOK: ChangeLook(); break;
	case Skeleton::STATE::SCARE: ChangeScare(); break;
	case Skeleton::STATE::END: ChangeEnd(); break;
	default:
		break;
	}
}

void Skeleton::ChangeIdle(void)
{
}

void Skeleton::ChangeLook(void)
{
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ENEMY_SKELETON_LOOK);
}

void Skeleton::ChangeScare(void)
{
	switch (side_)
	{
	case ENEMY_SIDE::RIGHT:
		info_.moveDir_ = { 0.0f, 0.0f, 1.0f };
		break;
	case ENEMY_SIDE::LEFT:
		info_.moveDir_ = { 0.0f, 0.0f, -1.0f };
		break;
	}

	info_.moveSpeed_ = 20.0f;

	anim_->Play(static_cast<int>(ANIM_TYPE::ATTACK), false);
}

void Skeleton::ChangeEnd(void)
{

}

void Skeleton::UpdateIdle(void)
{
	VECTOR playerPos = player_->GetTransform()->pos_;
	float distance = GetDistance(LOOK_POS, playerPos);
	if (distance <= TRIGGER_RANGE)
	{
		ChangeState(STATE::LOOK);
		return;
	}
}

void Skeleton::UpdateLook(void)
{
	VECTOR playerPos = player_->GetTransform()->pos_;
	float distance = GetDistance(SCARE_POS, playerPos);
	if (distance <= TRIGGER_RANGE)
	{
		ChangeState(STATE::SCARE);
		return;
	}

	LookPlayer();
}

void Skeleton::UpdateScare(void)
{
	Move();

	if (transform_)
	{
		// 進行方向に少し進んだ位置を計算
		VECTOR checkPos = VAdd(transform_->pos_, VScale(info_.moveDir_, 30.0f));
		VECTOR start = VAdd(checkPos, info_.startOffset_);
		VECTOR end = VAdd(checkPos, info_.endOffset_);

		// 壁との衝突チェック
		MV1_COLL_RESULT_POLY_DIM res = MV1CollCheck_Capsule(stageId_, -1, start, end, info_.radius_);

		if (res.HitNum > 0)
		{
			MV1CollResultPolyDimTerminate(res);

			AudioManager::GetInstance()->PlaySE(SoundID::SE_ENEMY_SKELETON);
			ChangeState(STATE::END);
			return;
		}
		MV1CollResultPolyDimTerminate(res);
	}
}

void Skeleton::UpdateEnd(void)
{

}