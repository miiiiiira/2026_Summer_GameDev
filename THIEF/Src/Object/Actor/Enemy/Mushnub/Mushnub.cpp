#include "../../../../Scene/SceneManager.h"
#include "../../../../Manager/Audio/AudioManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/AnimationController.h"
#include "../../../Component/PlayerController/PlayerController.h"
#include "../../../Component/Collider/3DCollider/CapsuleCollider.h"
#include "../../../Component/Collider/StageCollider/StageCollider.h"
#include "../../../Component/Animation/Animation.h"
#include "../../../Component/Transform/Transform.h"
#include "../EnemyCommon.h"
#include "Mushnub.h"

Mushnub::Mushnub(void)
{
}

Mushnub::~Mushnub(void)
{
}

void Mushnub::Init(void)
{
	EnemyBase::Init();

	anim_->Init();
	for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
	{
		anim_->AddInFbx(static_cast<int>(i), 0.2f, static_cast<int>(i));
	}

	const auto& data = EnemyTable::Table.at(ENEMY_TAG::MUSHNUB);
	SetEnemyData(data);

	// パラメータ初期化
	info_.moveDir_ = Math::VECTOR_ZERO;
	info_.viewRadius_ = 1000.0f;
	info_.attackDamagePow_ = 10.0f;
	info_.tag_ = ENEMY_TAG::MUSHNUB;

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

void Mushnub::Update(void)
{
	// 遅延回転処理
	DelayRotate();

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

	// ステート別更新
	switch (state_)
	{
	case Mushnub::STATE::IDLE:      UpdateIdle();     break;
	case Mushnub::STATE::SURPRISE:   UpdateSurprise(); break;
	case Mushnub::STATE::CHASE:     UpdateChase();    break;
	case Mushnub::STATE::HIT_REACT: UpdateHit();      break;
	case Mushnub::STATE::DEAD:      UpdateDead();     break;
	case Mushnub::STATE::END:       UpdateEnd();      break;
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
	if (anim_)
	{
		anim_->Update();
	}

	// モデルの更新
	MV1RefreshCollInfo(info_.modelId_, -1);
}

void Mushnub::Draw3D(void)
{
	EnemyBase::Draw3D();

#ifdef _DEBUG
	if (transform_)
	{
		VECTOR start = VAdd(transform_->pos_, info_.startOffset_);
		VECTOR end = VAdd(transform_->pos_, info_.endOffset_);
		DrawCapsule3D(start, end, info_.radius_, 8, 0xff0000, 0xff0000, false);
	}
	DrawCube3D(minAreaPos_, maxAreaPos_, 0xff00ff, 0xff00ff, false);
#endif
}

void Mushnub::Draw2D(void)
{
	DrawFormatString(10, 200, 0xffffff, "座標: (%.2f, %.2f, %.2f)", transform_->pos_.x, transform_->pos_.y, transform_->pos_.z);
}

void Mushnub::ChangeState(STATE state)
{
	state_ = state;

	switch (state_)
	{
	case Mushnub::STATE::IDLE: ChangeIdle(); break;
	case Mushnub::STATE::SURPRISE: ChangeSurprise(); break;
	case Mushnub::STATE::CHASE: ChangeChase(); break;
	case Mushnub::STATE::HIT_REACT: ChangeHit(); break;
	case Mushnub::STATE::DEAD: ChangeDead(); break;
	case Mushnub::STATE::END: ChangeEnd(); break;
	default:
		break;
	}
}

void Mushnub::ChangeIdle(void)
{
	info_.step_ = 5.0f;
	anim_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void Mushnub::ChangeSurprise(void)
{
	info_.step_ = 2.0f;
	LookPlayer();
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ENEMY_MUSHNUB, &transform_->pos_);
	anim_->Play(static_cast<int>(ANIM_TYPE::HIT_REACT), false);
}

void Mushnub::ChangeChase(void)
{
	info_.moveSpeed_ = 3.0f;
	anim_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
}

void Mushnub::ChangeHit(void)
{
}

void Mushnub::ChangeDead(void)
{
}

void Mushnub::ChangeEnd(void)
{
}

void Mushnub::UpdateIdle(void)
{
	if (IsPlayerInArea(minAreaPos_, maxAreaPos_))
	{
		ChangeState(STATE::SURPRISE);
		return;
	}
}

void Mushnub::UpdateSurprise(void)
{
	info_.step_ -= SceneManager::GetInstance()->GetDeltaTime();
	if (info_.step_ < 0.0f)
	{
		// 待機終了
		ChangeState(STATE::CHASE);
		return;
	}
}

void Mushnub::UpdateChase(void)
{
	if (IsPlayerInArea(minAreaPos_, maxAreaPos_))
	{
		LookPlayer();
	}
	else
	{
		// 相手へのベクトルを計算
		VECTOR diff = VSub(chasePos_, transform_->pos_);
		diff.y = 0.0f;

		// ベクトルの正規化で単位ベクトル（方向）を取得
		info_.moveDir_ = VNorm(diff);

		// 回転はY軸のみ
		transform_->angle_.x = transform_->angle_.z = 0.0f;

		float enemyDist2 = GetDistance(transform_->pos_, chasePos_);

		if (enemyDist2 <= 100.0f * 100.0f)
		{
			ChangeState(STATE::IDLE);
			return;
		}
	}

	float enemyDist = GetDistance(transform_->pos_, player_->GetTransform()->pos_);

	if (enemyDist >= 200.0f * 200.0f)
	{
		anim_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
		Move();
	}
	else
	{
		anim_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
	}

}

void Mushnub::UpdateHit(void)
{
}

void Mushnub::UpdateDead(void)
{
}

void Mushnub::UpdateEnd(void)
{
}