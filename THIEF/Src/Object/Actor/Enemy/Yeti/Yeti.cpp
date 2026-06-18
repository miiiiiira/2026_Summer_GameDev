#include "../../../../Application.h"
#include "../../../../Scene/SceneManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/AnimationController.h"
#include "../../../Component/PlayerController/PlayerController.h"
#include "../../../Component/Collider/3DCollider/CapsuleCollider.h"
#include "../../../Component/Transform/Transform.h"
#include "../Weapon/WeaponPunch.h"
#include "Yeti.h"


Yeti::Yeti(void)
	: 
	currentNodeId_(0),
	prevNodeId_(-1),
	prevPrevNodeId_(-1),
	patrolRadius_(0.0f),
	nextWayPoint_{0.0f, 0.0f, 0.0f},
	nextNodeId_(0),
	chaseTimer_(0.0f)
{
}

Yeti::~Yeti(void)
{
}

void Yeti::Init(PlayerController* player, int id)
{
	if (player == nullptr) return;
	player_ = player;

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

	startOffset_ = { 0.0f,180.0f,0.0f };
	endOffset_ = { 0.0f,40.0f,0.0f };
	radius_ = 50.0f;

	patrolRadius_ = 1500.0f;
	viewRadius_ = 1000.0f;

	isGround_ = false;

	// 武器の初期化
	weaponPunch_ = new WeaponPunch();
	weaponPunch_->Init(WeaponBase::TYPE::PUNCH);

	// Yetiの攻撃ははパンチ
	useWeapon_ = weaponPunch_;

	// 初期アニメーション再生
	animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);

	candidates_.reserve(way_.size());

	if (id == -1) return;
	stageId_ = id;

	edgeList_.clear();
	edgeList_.resize(way_.size());

	isNotice_ = false;
	isHit_ = false;

	for (int i = 0; i < static_cast<int>(way_.size()); i++)
	{
		for (int j = 0; j < static_cast<int>(way_.size()); j++)
		{
			// 同じノードならスキップ
			if (i == j) continue;


			// 遠すぎるノードも除外する
			float nodeDistance = GetDistance(way_[j].pos, way_[i].pos);
			if (nodeDistance > NODE_CONNECT_MAX_DISTANCE_SQ) continue;

			AddEdge(i, j);
		}
	}

	currentNodeId_ = FindNearestNode(pos_);

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
	prevPos_ = pos_;
	// 遅延回転処理
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
	case Yeti::STATE::SURPRISE: UpdateSurprise(); break;
	case Yeti::STATE::CHASE: UpdateChase(); break;
	case Yeti::STATE::ATTACK: UpdateAttack(); break;
	case Yeti::STATE::HIT_REACT: UpdateHit(); break;
	case Yeti::STATE::DEAD: UpdateDead(); break;
	case Yeti::STATE::END: UpdateEnd(); break;
	default:
		break;
	}

	// 重力処理
	ApplyGravity();

	// アニメーションの更新
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
		if (point.id == prevNodeId_)
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
	useWeapon_->Draw();
#endif
}

Yeti::STATE Yeti::GetState(void)
{
	return state_;
}

void Yeti::SetMoveDirPatrol(void)
{
	VECTOR tmpPos = nextWayPoint_;
	tmpPos.y = 0.0f;

	VECTOR pos = pos_;
	pos.y = 0.0f;

	moveDir_ = VNorm(VSub(tmpPos, pos));
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

int Yeti::SelectNextNode(void)
{
	// 有効ノードを探す前に空にする
	candidates_.clear();

	for (const auto& edge : edgeList_[currentNodeId_])
	{
		int nextId = edge.way.id;

		float distance = GetDistance(edge.way.pos, pos_);

		// 敵の座標から半径以内に無いポイントは除外
		if (distance > patrolRadius_ * patrolRadius_) continue;

		// 前回、前々回のノードは除外する
		if (nextId == prevNodeId_) continue;
		if (nextId == prevPrevNodeId_) continue;

		candidates_.push_back(nextId);
	}

	// 候補があった場合ランダムに選ぶ
	if (!candidates_.empty())
	{
		int index = GetRand(static_cast<int>(candidates_.size() - 1));
		return candidates_[index];
	}

	if (prevNodeId_ != -1)
	{
		return prevNodeId_;
	}

	return currentNodeId_;
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

int Yeti::FindNearestNode(VECTOR pos)
{
	int nearNodeId = 0;
	float minCost = FLT_MAX;
	for (const auto& way : way_)
	{
		// 一番近いノードを探す
		float distance = VSize(VSub(pos, way.pos));

		if (distance < minCost)
		{
			minCost = distance;
			nearNodeId = way.id;
		}
	}
	return nearNodeId;
}

void Yeti::ChaseNode(void)
{
	nextWayPoint_ = path_[nextNodeId_].way.pos;
	// 移動方向を設定
	SetMoveDirPatrol();

	VECTOR enemyPos = pos_;
	enemyPos.y = 0.0f;

	VECTOR targetNode = path_[nextNodeId_].way.pos;
	targetNode.y = 0.0f;

	// 水平方向の純粋な距離を測る
	float nodeDistance = VSize(VSub(targetNode, enemyPos));

	if (nodeDistance < 60.0f)
	{
		nextNodeId_++;
	}
}

void Yeti::ChaseDirect(void)
{
	LookPlayer();
}

bool Yeti::CheckChaseLineCollision(VECTOR pPos, VECTOR ePos, float radius)
{
	// 線分とモデルの衝突判定
	MV1_COLL_RESULT_POLY_DIM res = MV1CollCheck_Capsule(stageId_, -1, pPos, ePos, radius);

	// 当たっていたら、trueを返す
	if (res.HitNum > 0)
	{
		MV1CollResultPolyDimTerminate(res);
		return true;
	}
	MV1CollResultPolyDimTerminate(res);
	return false;
}

void Yeti::ChangeState(STATE state)
{
	state_ = state;

	switch (state_)
	{
	case Yeti::STATE::THINK: ChangeThink(); break;
	case Yeti::STATE::IDLE: ChangeIdle(); break;
	case Yeti::STATE::PATROL: ChangePatrol(); break;
	case Yeti::STATE::SURPRISE: ChangeSurprise(); break;
	case Yeti::STATE::CHASE: ChangeChase(); break;
	case Yeti::STATE::ATTACK: ChangeAttack(); break;
	case Yeti::STATE::HIT_REACT: ChangeHit(); break;
	case Yeti::STATE::DEAD: ChangeDead(); break;
	case Yeti::STATE::END: ChangeEnd(); break;
	default:
		break;
	}
}

void Yeti::ChangeThink(void)
{
}

void Yeti::ChangeIdle(void)
{
	step_ = 5.0f;
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
	moveSpeed_ = 4.0f;

	animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
}

void Yeti::ChangeSurprise(void)
{
	step_ = 2.0f;
	LookPlayer();
	animationController_->Play(static_cast<int>(ANIM_TYPE::HIT_REACT), false);
}

void Yeti::ChangeChase(void)
{
	moveSpeed_ = 8.0f;
	chaseTimer_ = 0.0f;
	targetLostTimer_ = 0.0f;
	isNotice_ = false;
	animationController_->Play(static_cast<int>(ANIM_TYPE::RUN), true);
}

void Yeti::ChangeAttack(void)
{
	LookPlayer();
	useWeapon_->Use(pos_, moveDir_);
	animationController_->Play(static_cast<int>(ANIM_TYPE::PUNCH), false);
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

void Yeti::UpdateIdle(void)
{
	if (CheckPlayerDiscovery(viewRadius_))
	{
		ChangeState(STATE::SURPRISE);
		return;
	}

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
	if (CheckPlayerDiscovery(viewRadius_))
	{
		ChangeState(STATE::SURPRISE);
		return;
	}

	// 目的地までの距離を測る
	VECTOR target = VSub(nextWayPoint_, pos_);
	target.y = 0.0f;
	float dist = VSquareSize(target);

	// ある程度近づいたら考えるステートに移る
	if (dist < 50.0f * 50.0f)
	{
		ChangeState(STATE::THINK);
		return;
	}

	Move();
}

void Yeti::UpdateSurprise(void)
{
	step_ -= SceneManager::GetInstance()->GetDeltaTime();
	if (step_ < 0.0f)
	{
		// 待機終了
		ChangeState(STATE::CHASE);
		return;
	}
}

void Yeti::UpdateChase(void)
{
	VECTOR enemyPos = pos_;
	VECTOR playerPos = player_->GetTransform()->pos_;

	// 視線位置
	VECTOR enemyHead = VAdd(pos_, startOffset_);
	VECTOR playerHead = player_->GetCapsule()->GetStart();
	float distance = VSize(VSub(playerPos, enemyPos));

	// 視線チェック
	bool isPlayerVisible = false;

	// 頭位置の高さの差を計算する
	float heightDiff = fabsf(playerHead.y - enemyHead.y);

	// 高低差が50.0fより大きければ、見えない判定にする
	if (heightDiff > 50.0f || distance > viewRadius_)
	{
		isPlayerVisible = false;
		isHit_ = true;
	}
	else
	{
		// 高低差が範囲内なら、視線チェックする
		isPlayerVisible = !CheckChaseLineCollision(enemyHead, playerHead, 20.0f);
		isHit_ = !isPlayerVisible;
	}

	// プレイヤーを見つけたなら
	if (isPlayerVisible)
	{
		// 見失いタイマーをリセット
		targetLostTimer_ = 0.0f;

		//　攻撃範囲内にいたら、攻撃状態にする
		if (distance <= 300)
		{
			ChangeState(STATE::ATTACK);
			return;
		}
	}
	else
	{
		// 見失いタイマーをカウントさせる
		targetLostTimer_ += SceneManager::GetInstance()->GetDeltaTime();
	}

	// タイマーがリミットより多くなったら、近くのノードを探して
	// そこから巡回をさせる
	if (targetLostTimer_ >= LOST_LIMIT_TIME)
	{
		path_.clear();
		nextNodeId_ = 0;

		currentNodeId_ = FindNearestNode(pos_);
		prevNodeId_ = -1;
		prevPrevNodeId_ = -1;
		nextWayPoint_ = way_[currentNodeId_].pos;

		ChangeState(STATE::IDLE);
		return;
	}

	// タイマーを進める
	chaseTimer_ += SceneManager::GetInstance()->GetDeltaTime();

	if (chaseTimer_ >= CHASE_INTERVAL)
	{
		if (isHit_ && path_.empty())
		{
			// プレイヤーから一番近いノードを探す
			int playerNearNodeId = FindNearestNode(playerPos);
			// 敵から一番近いノードを探す
			int enemyNearNodeId = FindNearestNode(enemyPos);
			// 敵の位置とプレイヤーの位置を繋ぐルートを探す
			FindPath(enemyNearNodeId, playerNearNodeId);

			if (path_.size() > 1) 
			{ 
				nextNodeId_ = 1; 
			}
			else 
			{ 
				nextNodeId_ = 0; 
			}
		}

		// 判定が終わったらタイマーをリセットする
		chaseTimer_ = 0.0f;
	}

	if (path_.empty())
	{
		// ルートがないなら、直接追従
		ChaseDirect();
	}
	else
	{
		// 範囲外チェック
		// 最後のノードにたどり着いたかチェックする
		if (nextNodeId_ >= static_cast<int>(path_.size()))
		{
			path_.clear();
			nextNodeId_ = 0;

			if (!isPlayerVisible)
			{
				currentNodeId_ = FindNearestNode(pos_);
				prevNodeId_ = -1;
				prevPrevNodeId_ = -1;
				nextWayPoint_ = way_[currentNodeId_].pos;
				LookPlayer();

				ChangeState(STATE::IDLE);
				return;
			}
		}
		else
		{
			// ルートがあるなら、次のノードがあるか確認してからルート追従
			ChaseNode();
		}
	}
	Move();
}

void Yeti::UpdateAttack(void)
{
	// 攻撃処理の更新
	useWeapon_->Update();

	if (animationController_->IsEnd())
	{
		useWeapon_->SetAlive(false);
		ChangeState(STATE::CHASE);
		return;
	}
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