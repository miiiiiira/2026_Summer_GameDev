#include "../../../../Application.h"
#include "../../../../Scene/SceneManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/AnimationController.h"
#include "Yeti.h"


Yeti::Yeti(void)
	: 
	currentNodeId_(0),
	prevNodeId_(-1),
	prevPrevNodeId_(-1),
	patrolRadius_(0.0f),
	nextWayPoint_{0.0f, 0.0f, 0.0f}
{
}

Yeti::~Yeti(void)
{
}

void Yeti::Init(int id)
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

	patrolRadius_ = 2000.0f;
	viewRadius_ = 1000.0f;

	// 初期アニメーション再生
	animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);

	candidates_.reserve(way_.size());

	if (id == -1) return;
	stageId_ = id;

	edgeList_.clear();
	edgeList_.resize(way_.size());

	for (int i = 0; i < static_cast<int>(way_.size()); i++)
	{
		for (int j = 0; j < static_cast<int>(way_.size()); j++)
		{
			// 同じノードならスキップ
			if (i == j) continue;

			AddEdge(i, j);
		}
	}

	ChangeState(STATE::IDLE);
}

void Yeti::Load(void)
{
	EnemyBase::Load();

	modelId_ = MV1LoadModel((Application::PATH_MODEL + "Enemy/Yeti.mv1").c_str());

	// モデルアニメーション制御の初期化
	animationController_ = new AnimationController(modelId_);
	for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
	{
		animationController_->AddInFbx(i, 0.5f, i);
	}

}

void Yeti::Update(void)
{
	// プレイヤーの遅延回転処理
	DelayRotate();

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = Matrix::Multiplication(localAngle_, angle_);
	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	switch (state_)
	{
	case Yeti::STATE::THINK: UpdateThink(); break;
	case Yeti::STATE::IDLE: UpdateIdle(); break;
	case Yeti::STATE::PATROL: UpdatePatrol(); break;
	case Yeti::STATE::CHASE: UpdateChase(); break;
	case Yeti::STATE::ATTACK: UpdateAttack(); break;
	case Yeti::STATE::HIT_REACT: UpdateHit(); break;
	case Yeti::STATE::DEAD: UpdateDead(); break;
	case Yeti::STATE::END: UpdateEnd(); break;
	default:
		break;
	}

	animationController_->Update();
}

void Yeti::Draw(void)
{
	EnemyBase::Draw();

#ifdef _DEBUG
	DrawSphere3D(pos_, patrolRadius_, 8, GetColor(0, 255, 0), GetColor(0, 0, 0), FALSE);

	// 巡回ルート描画
	for (const auto& point : way_)
	{
		float distance = VSize(VSub(point.pos, pos_));

		unsigned int color = 0x0000ff;
		if (point.id == nextNodeId_)
		{
			color = 0x006400;
		}
		else if (point.id == prevNodeId_)
		{
			color = 0xff8c00;
		}
		else if (point.id == prevPrevNodeId_)
		{
			color = 0xfff5ee;
		}
		else if (distance > patrolRadius_)
		{
			color = 0xff0000;
		}

		DrawSphere3D(
			point.pos, 50.0f, 10,
			color, color, false);
	}
#endif
}

void Yeti::SetMoveDirPatrol(void)
{
	VECTOR tmpPos = nextWayPoint_;
	tmpPos.y = 0.0f;

	VECTOR pos = pos_;
	pos.y = 0.0f;

	moveDir_ = VNorm(VSub(tmpPos, pos));
}

int Yeti::SelectNextNode(void)
{
	// 有効ノードを探す前に空にする
	candidates_.clear();

	for (const auto& edge : edgeList_[currentNodeId_])
	{
		int nextId = edge.way.id;

		float distance = VSize(VSub(edge.way.pos, pos_));

		// 敵の座標から半径以内に無いポイントは除外
		if (distance > patrolRadius_) continue;

		// 前回、前々回のノードは除外する
		if (nextId == prevNodeId_) continue;
		if (nextId == prevPrevNodeId_) continue;

		candidates_.push_back(nextId);
	}

	// 候補があった場合ランダムに選ぶ
	if (!candidates_.empty())
	{
		int index = GetRand(static_cast<int>(candidates_.size() - 1));
		nextNodeId_ = candidates_[index];
		return candidates_[index];
	}

	if (prevNodeId_ != -1)
	{
		nextNodeId_ = prevNodeId_;
		return prevNodeId_;
	}

	nextNodeId_ = currentNodeId_;
	return currentNodeId_;
}

void Yeti::ArriveNode(void)
{
	// 次のノードを選ぶ
	int nextId = SelectNextNode();

	// 履歴を更新する
	prevPrevNodeId_ = prevNodeId_;
	prevNodeId_ = currentNodeId_;
	currentNodeId_ = nextId;

	// 次の目的地の座標を設定する
	nextWayPoint_ = way_[currentNodeId_].pos;
}

void Yeti::Move(void)
{
	// 移動量を計算する
	movePow_ = VScale(moveDir_, moveSpeed_);
	// 移動量処理
	pos_ = VAdd(pos_, movePow_);
	// モデルに座標を設定
	MV1SetPosition(modelId_, pos_);
}

void Yeti::ChangeState(STATE state)
{
	state_ = state;

	switch (state_)
	{
	case Yeti::STATE::THINK:
		ChangeThink();
		break;
	case Yeti::STATE::IDLE:
		ChangeIdle();
		break;
	case Yeti::STATE::PATROL:
		ChangePatrol();
		break;
	case Yeti::STATE::CHASE:
		ChangeChase();
		break;
	case Yeti::STATE::ATTACK:
		ChangeAttack();
		break;
	case Yeti::STATE::HIT_REACT:
		ChangeHit();
		break;
	case Yeti::STATE::DEAD:
		ChangeDead();
		break;
	case Yeti::STATE::END:
		ChangeEnd();
		break;
	default:
		break;
	}
}

void Yeti::ChangeThink(void)
{
	// 思考
	// ランダムに次の行動を決定
	// 20%で待機、80%で徘徊
	int rand = GetRand(100);
	if (rand < 20)
	{
		ChangeState(STATE::IDLE);
	}
	else
	{
		ChangeState(STATE::PATROL);
	}
}

void Yeti::ChangeIdle(void)
{
	step_ = 3.0f;
	animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void Yeti::ChangePatrol(void)
{
	// 移動量ゼロ
	movePow_ = { 0.0f, 0.0f, 0.0f };

	int lastNodeId = currentNodeId_;
	
	ArriveNode();

	if (currentNodeId_ == lastNodeId)
	{
		ChangeState(STATE::IDLE);
		return;
	}

	// 移動方向を設定
	SetMoveDirPatrol();

	// 移動スピード
	moveSpeed_ = 5.0f;

	animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);

}

void Yeti::ChangeChase(void)
{
}

void Yeti::ChangeAttack(void)
{
}

void Yeti::ChangeHit(void)
{
}

void Yeti::ChangeDead(void)
{
}

void Yeti::ChangeEnd(void)
{
}


void Yeti::UpdateThink(void)
{
}

void Yeti::UpdateIdle(void)
{
	step_ -= SceneManager::GetInstance()->GetDeltaTime();

	if (step_ < 0.0f)
	{
		// 待機終了
		ChangeState(STATE::THINK);
		return;
	}
}

void Yeti::UpdatePatrol(void)
{
	// 目的地までの距離を測る
	VECTOR target = VSub(nextWayPoint_, pos_);
	target.y = 0.0f;
	float dist = VSize(target);

	// ある程度近づいたら考えるステートに移る
	if (dist < 50.0f)
	{
		ChangeState(STATE::THINK);
		return;
	}

	Move();
}

void Yeti::UpdateChase(void)
{
	LookPlayer();
	Move();
}

void Yeti::UpdateAttack(void)
{
}

void Yeti::UpdateHit(void)
{
}

void Yeti::UpdateDead(void)
{
}

void Yeti::UpdateEnd(void)
{
}