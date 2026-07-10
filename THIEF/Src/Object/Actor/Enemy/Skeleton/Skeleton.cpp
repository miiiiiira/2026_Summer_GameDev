#include "../../../../Scene/SceneManager.h"
#include "../../../../Common/Manager/Audio/AudioManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/AnimationController.h"
#include "../../../Common/AnimationController.h"
#include "../../../Component/PlayerController/PlayerController.h"
#include "../../../Component/Collider/3DCollider/CapsuleCollider.h"
#include "../../../Component/Transform/Transform.h"
#include "Skeleton.h"

Skeleton::Skeleton(int modelId)
	:
	EnemyBase(-1)
{
	baseModelId_ = modelId;
}

Skeleton::~Skeleton(void)
{
}

void Skeleton::OnInitialize(void)
{
	scale_ = SCALE;
	MV1SetScale(modelId_, scale_);


	localAngle_ = { 0.0f, Math::Deg2Rad(180.0f), 0.0f };
	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = Matrix::Multiplication(localAngle_, angle_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	MV1SetPosition(modelId_, pos_);
	prevPos_ = pos_;

	moveDir_ = { 0.0f, 0.0f, 0.0f };

	startOffset_ = { 0.0f,80.0f,0.0f };
	endOffset_ = { 0.0f,40.0f,0.0f };
	radius_ = 50.0f;

	isGround_ = false;

	ChangeState(STATE::IDLE);
}

void Skeleton::Load(void)
{
	modelId_ = MV1DuplicateModel(baseModelId_);
	// モデルアニメーション制御の初期化
	animationController_ = new AnimationController(modelId_);
	for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
	{
		animationController_->AddInFbx(i, 1.0f, i);
	}
}

void Skeleton::Update(void)
{
	prevPos_ = pos_;

	if (state_ != STATE::IDLE)
	{
		// 遅延回転処理
		DelayRotate();

		// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
		MATRIX mat = Matrix::Multiplication(localAngle_, angle_);
		// 回転行列をモデルに反映
		MV1SetRotationMatrix(modelId_, mat);
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
	animationController_->Update();
}

void Skeleton::Draw(void)
{
	EnemyBase::Draw();
}

void Skeleton::SetSide(SIDE side)
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
	case SIDE::RIGHT:
		moveDir_ = { 0.0f, 0.0f, 1.0f };
		break;
	case SIDE::LEFT:
		moveDir_ = { 0.0f, 0.0f, -1.0f };
		break;
	}

	moveSpeed_ = 20.0f;

	animationController_->Play(static_cast<int>(ANIM_TYPE::ATTACK), false);
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

	if (isCollisionStage_)
	{
		AudioManager::GetInstance()->PlaySE(SoundID::SE_ENEMY_SKELETON);
		ChangeState(STATE::END);
		return;
	}
}

void Skeleton::UpdateEnd(void)
{

}