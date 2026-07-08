#include "../../../../Scene/SceneManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../../../Common/Transform/MatrixUtility.h"
#include "../../../Component/PlayerController/PlayerController.h"
#include "../../../Component/Collider/3DCollider/CapsuleCollider.h"
#include "../../../Component/Transform/Transform.h"
#include "../Weapon/WeaponPunch.h"
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
	endOffset_ = { 0.0f,80.0f,0.0f };
	radius_ = 80.0f;

	isGround_ = false;

	attackMoveSpeed_ = 20.0f;
	attackJumpPow_ = 25.0f;
	attackDamagePow_ = 80.0f;

	// 武器の初期化
	weaponPunch_ = new WeaponPunch();
	weaponPunch_->Init(WeaponBase::TYPE::PUNCH);

	// Statueの攻撃はパンチ
	useWeapon_ = weaponPunch_;


	candidates_.reserve(way_->size());

	currentNodeId_ = FindNearestNode(pos_);

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
	case Statue::STATE::SURPRISE: UpdateSurprise(); break;
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

	DrawCube3D(MIN_AREA_POS, MAX_AREA_POS, 0xff00ff, 0xff00ff, false);


	for (int i = 0; i < (int)edgeList_->size(); i++)
	{
		for (const auto& edge : (*edgeList_)[i])
		{
			DrawLine3D((*way_)[i].pos, edge.way.pos, GetColor(255, 255, 0));
		}
	}

	// 現在地から、今目指しているノード（currentNodeId_）までの線を引く
	DrawLine3D(pos_, (*way_)[currentNodeId_].pos, GetColor(255, 0, 255));

	DrawSphere3D(pos_, patrolRadius_, 8, GetColor(0, 255, 0), GetColor(0, 0, 0), FALSE);

	// 1. 今の目的地（行きたい場所）を「緑」で描画
	DrawSphere3D(nextWayPoint_, 40.0f, 10, GetColor(0, 255, 0), GetColor(0, 255, 0), TRUE);

	// 2. 敵の現在地から、緑の目的地へ向かって「線」を引く
	DrawLine3D(pos_, nextWayPoint_, GetColor(0, 255, 0));

	// 巡回ルート描画
	for (const auto& point : (*way_))
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

	VECTOR enemyPos = pos_;
	VECTOR playerPos = player_->GetTransform()->pos_;


	DrawLine3D(enemyPos, playerPos, 0x00ff00);

	DrawSphere3D(enemyPos, 80.0f, 8, 0xff00ff, 0xff00ff, false);
	enemyPos.y += 100.0f;
	DrawSphere3D(enemyPos, 80.0f, 8, 0xff0000, 0xff0000, false);
	enemyPos.y += 100.0f;
	DrawSphere3D(enemyPos, 80.0f, 8, 0x00ff00, 0x00ff00, false);
	enemyPos.y += 100.0f;
	DrawSphere3D(enemyPos, 80.0f, 8, 0x00ffff, 0x00ffff, false);
	enemyPos.y += 100.0f;
	DrawSphere3D(enemyPos, 80.0f, 8, 0xffff00, 0xffff00, false);


	useWeapon_->Draw();

#endif
}

void Statue::ChangeState(STATE state)
{
	state_ = state;

	switch (state_)
	{
	case Statue::STATE::IDLE: ChangeIdle(); break;
	case Statue::STATE::SURPRISE: ChangeSurprise(); break;
	case Statue::STATE::CHASE: ChangeChase(); break;
	case Statue::STATE::ATTACK: ChangeAttack(); break;
	case Statue::STATE::END: ChangeEnd(); break;
	default:
		break;
	}
}

void Statue::ChangeIdle(void)
{
}

void Statue::ChangeSurprise(void)
{
	step_ = 4.0f;
}

void Statue::ChangeChase(void)
{
	moveSpeed_ = 5.0f;
}

void Statue::ChangeAttack(void)
{
	step_ = 10.0f;
	LookPlayer();
	VECTOR enemyAttackPos = pos_;
	enemyAttackPos.y += 80.0f;
	useWeapon_->Use(enemyAttackPos, moveDir_);
}

void Statue::ChangeEnd(void)
{
}

void Statue::UpdateIdle(void)
{
	if (IsPlayerInArea(MIN_AREA_POS, MAX_AREA_POS))
	{
		ChangeState(STATE::SURPRISE);
		return;
	}
}

void Statue::UpdateSurprise(void)
{
	step_ -= SceneManager::GetInstance()->GetDeltaTime();
	if (step_ < 0.0f)
	{
		// 待機終了
		ChangeState(STATE::CHASE);
		return;
	}
}

void Statue::UpdateChase(void)
{
	VECTOR enemyPos = pos_;
	VECTOR playerPos = player_->GetTransform()->pos_;

	// 視線位置
	VECTOR enemyHead = VAdd(pos_, startOffset_);
	VECTOR playerHead = player_->GetCapsule()->GetStart();
	float distance = VSize(VSub(playerPos, enemyPos));

	// プレイヤーがエリア内にいるなら
	if (IsPlayerInArea(MIN_AREA_POS, MAX_AREA_POS))
	{
		// 視線チェック
		bool isPlayerVisible = !CheckChaseLineCollision(enemyHead, playerHead, 40.0f);

		// 画面内に入っているかをチェックする
		bool isLookedByPlayer1 = !CheckCameraViewClip(enemyPos);
		enemyPos.y += 100.0f;
		bool isLookedByPlayer2 = !CheckCameraViewClip(enemyPos);
		enemyPos.y += 100.0f;
		bool isLookedByPlayer3 = !CheckCameraViewClip(enemyPos);
		enemyPos.y += 100.0f;
		bool isLookedByPlayer4 = !CheckCameraViewClip(enemyPos);
		enemyPos.y += 100.0f;
		bool isLookedByPlayer5 = !CheckCameraViewClip(enemyPos);

		//　攻撃範囲内にいたら、攻撃状態にする
		if (distance <= 200.0f)
		{
			ChangeState(STATE::ATTACK);
			return;
		}

		// プレイヤーに見られているなら
		if (isLookedByPlayer1 || isLookedByPlayer2 || isLookedByPlayer3 || isLookedByPlayer4 || isLookedByPlayer5)
		{
			path_.clear();
			return;
		}
		else if (!isPlayerVisible)
		{
			if (path_.empty())
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
			else
			{
				if (nextNodeId_ >= static_cast<int>(path_.size()))
				{
					path_.clear(); 
					nextNodeId_ = 0;
				}
				else
				{
					ChaseNode(); 
				}
			}
		}
		else
		{
			path_.clear();
			ChaseDirect();
		}
	}
	else
	{
		// 見られていないなら帰還開始
		if (CheckCameraViewClip(enemyPos)) // true なら見られていない
		{
			// 帰還先との間に障害物があるかチェック
			if (CheckChaseLineCollision(pos_, DEFAULT_POS, 40.0f))
			{
				// 障害物あり
				if (path_.empty())
				{
					int enemyNearNode = FindNearestNode(pos_);
					int defaultNearNode = FindNearestNode(DEFAULT_POS);
					FindPath(enemyNearNode, defaultNearNode);
					if (path_.size() > 1)
					{
						nextNodeId_ = 1;
					}
					else
					{
						nextNodeId_ = 0;
					}
				}

				if (nextNodeId_ >= static_cast<int>(path_.size())) 
				{
					path_.clear();
					nextNodeId_ = 0;
				}
				else
				{
					ChaseNode();
				}
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
		}
		else
		{
			moveDir_ = { 0.0f, 0.0f, 0.0f };
		}
	}

	Move();

	// 重力処理
	ApplyGravity();

}

void Statue::UpdateAttack(void)
{

	step_ -= SceneManager::GetInstance()->GetDeltaTime();

	// 攻撃処理の更新
	useWeapon_->Update();


	if (step_ < 0.0f)
	{
		useWeapon_->SetAlive(false);
		ChangeState(STATE::CHASE);
		return;
	}
}

void Statue::UpdateEnd(void)
{
}