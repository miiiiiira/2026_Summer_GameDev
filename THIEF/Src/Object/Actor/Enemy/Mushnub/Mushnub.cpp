#include "../../../../Application.h"
#include "../../../../Scene/SceneManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/AnimationController.h"
#include "../../../Component/PlayerController/PlayerController.h"
#include "../../../Component/Collider/3DCollider/CapsuleCollider.h"
#include "../../../Component/Transform/Transform.h"
#include "Mushnub.h"

Mushnub::Mushnub(int modelId)
	:
	EnemyBase(modelId)
{
}

Mushnub::~Mushnub(void)
{
}

void Mushnub::OnInitialize(void)
{
	scale_ = SCALE;
	MV1SetScale(modelId_, scale_);

	angle_ = DEFAULT_ANGLE;
	localAngle_ = { 0.0f, Math::Deg2Rad(180.0f), 0.0f };
	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = Matrix::Multiplication(localAngle_, angle_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	pos_ = DEFAULT_POS;
	MV1SetPosition(modelId_, pos_);
	prevPos_ = pos_;

	moveDir_ = { 0.0f, 0.0f, 0.0f };

	startOffset_ = { 0.0f,80.0f,0.0f };
	endOffset_ = { 0.0f,40.0f,0.0f };
	radius_ = 50.0f;

	viewRadius_ = 1000.0f;

	isGround_ = false;

	// 初期アニメーション再生
	animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);

	ChangeState(STATE::IDLE);
}

void Mushnub::Load(void)
{
	// モデルアニメーション制御の初期化
	animationController_ = new AnimationController(modelId_);
	for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
	{
		animationController_->AddInFbx(i, 0.5f, i);
	}
}

void Mushnub::Update(void)
{
	prevPos_ = pos_;
	// 遅延回転処理
	DelayRotate();

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = Matrix::Multiplication(localAngle_, angle_);
	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	switch (state_)
	{
	case Mushnub::STATE::IDLE: UpdateIdle(); break;
	case Mushnub::STATE::SURPRISE: UpdateSurprise(); break;
	case Mushnub::STATE::CHASE: UpdateChase(); break;
	case Mushnub::STATE::HIT_REACT: UpdateHit(); break;
	case Mushnub::STATE::DEAD: UpdateDead(); break;
	case Mushnub::STATE::END: UpdateEnd(); break;
	default:
		break;
	}

	// 重力処理
	ApplyGravity();

	// アニメーションの更新
	animationController_->Update();
}

void Mushnub::Draw(void)
{
	EnemyBase::Draw();

#ifdef _DEBUG

	VECTOR start = VAdd(pos_, startOffset_);
	VECTOR end = VAdd(pos_, endOffset_);
	DrawCapsule3D(start, end, radius_, 8, 0xff0000, 0xff0000, false);

	DrawCube3D(MIN_AREA_POS, MAX_AREA_POS, 0xff00ff, 0xff00ff, false);

#endif
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
	step_ = 5.0f;
	animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void Mushnub::ChangeSurprise(void)
{
	step_ = 2.0f;
	LookPlayer();
	animationController_->Play(static_cast<int>(ANIM_TYPE::HIT_REACT), false);
}

void Mushnub::ChangeChase(void)
{
	moveSpeed_ = 3.0f;
	animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
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
	if (IsPlayerInArea(MIN_AREA_POS, MAX_AREA_POS))
	{
		ChangeState(STATE::SURPRISE);
		return;
	}
}

void Mushnub::UpdateSurprise(void)
{
	step_ -= SceneManager::GetInstance()->GetDeltaTime();
	if (step_ < 0.0f)
	{
		// 待機終了
		ChangeState(STATE::CHASE);
		return;
	}
}

void Mushnub::UpdateChase(void)
{
	if (IsPlayerInArea(MIN_AREA_POS, MAX_AREA_POS))
	{
		LookPlayer();
	}
	else
	{
		// 相手へのベクトルを計算
		VECTOR diff = VSub(CHASE_POS, pos_);
		diff.y = 0.0f;

		// ベクトルの正規化で単位ベクトル（方向）を取得
		moveDir_ = VNorm(diff);

		// 回転はY軸のみ
		angle_.x = angle_.z = 0.0f;

		float enemyDist2 = GetDistance(pos_, CHASE_POS);

		if (enemyDist2 <= 100.0f * 100.0f)
		{
			ChangeState(STATE::IDLE);
			return;
		}
	}

	float enemyDist = GetDistance(pos_, player_->GetTransform()->pos_);

	if (enemyDist >= 200.0f * 200.0f)
	{
		animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
		Move();
	}
	else
	{
		animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
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