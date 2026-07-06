#include "../../../../Scene/SceneManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../../../Common/Transform/MatrixUtility.h"
#include "../../../Component/PlayerController/PlayerController.h"
#include "../../../Component/Transform/Transform.h"
#include "Statue.h"

Statue::Statue(int modelId)
	:
	EnemyBase(modelId)
{
}

Statue::~Statue(void)
{
}

void Statue::OnInitialize(void)
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

	startOffset_ = { 0.0f,400.0f,0.0f };
	endOffset_ = { 0.0f,90.0f,0.0f };
	radius_ = 80.0f;

	isGround_ = false;

	ChangeState(STATE::IDLE);
}

void Statue::Load(void)
{
}

void Statue::Update(void)
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
	case Statue::STATE::IDLE: UpdateIdle(); break;
	case Statue::STATE::CHASE: UpdateChase(); break;
	case Statue::STATE::ATTACK: UpdateAttack(); break;
	case Statue::STATE::END: UpdateEnd(); break;
	default:
		break;
	}
}

void Statue::Draw(void)
{
	EnemyBase::Draw();

#ifdef _DEBUG

	/*VECTOR playerPos = player_->GetTransform()->pos_;
	printfDx("playerPos %.2f, %.2f, %.2f" ,playerPos.x, playerPos.y, playerPos.z);*/

	VECTOR start = VAdd(pos_, startOffset_);
	VECTOR end = VAdd(pos_, endOffset_);
	DrawCapsule3D(start, end, radius_, 8, 0xff0000, 0xff0000, false);

	DrawCube3D(MIN_AREA_POS, MAX_AREA_POS, 0xff00ff, 0xff00ff, false);

#endif
}

void Statue::ChangeState(STATE state)
{
	state_ = state;

	switch (state_)
	{
	case Statue::STATE::IDLE: ChangeIdle(); break;
	case Statue::STATE::CHASE: ChangeChase(); break;
	case Statue::STATE::ATTACK: ChangeAttack(); break;
	case Statue::STATE::END: ChangeEnd(); break;
	default:
		break;
	}
}

void Statue::ChangeIdle(void)
{
	step_ = 5.0f;
}

void Statue::ChangeChase(void)
{
	moveSpeed_ = 5.0f;
}

void Statue::ChangeAttack(void)
{
}

void Statue::ChangeEnd(void)
{
}

void Statue::UpdateIdle(void)
{
	if (IsPlayerInArea(MIN_AREA_POS, MAX_AREA_POS))
	{
		ChangeState(STATE::CHASE);
		return;
	}
}

void Statue::UpdateChase(void)
{
	if (IsPlayerInArea(MIN_AREA_POS, MAX_AREA_POS))
	{
		LookPlayer();
	}
	else
	{
		// 相手へのベクトルを計算
		VECTOR diff = VSub(DEFAULT_POS, pos_);
		diff.y = 0.0f;

		// ベクトルの正規化で単位ベクトル（方向）を取得
		moveDir_ = VNorm(diff);

		// 回転はY軸のみ
		angle_.x = angle_.z = 0.0f;

		float enemyDist2 = GetDistance(pos_, DEFAULT_POS);

		if (enemyDist2 <= 100.0f * 100.0f)
		{
			ChangeState(STATE::IDLE);
			return;
		}
	}

	float enemyDist = GetDistance(pos_, player_->GetTransform()->pos_);

	if (enemyDist >= 200.0f * 200.0f)
	{
		Move();
	}
	else
	{
	}

	// 重力処理
	ApplyGravity();

}

void Statue::UpdateAttack(void)
{
}

void Statue::UpdateEnd(void)
{
}