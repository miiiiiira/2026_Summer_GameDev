#include "../../../../Scene/SceneManager.h"
#include "../../../../Manager/Audio/AudioManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../../../Common/Transform/MatrixUtility.h"
#include "../../../Component/PlayerController/PlayerController.h"
#include "../../../Component/Collider/3DCollider/CapsuleCollider.h"
#include "../../../Component/Transform/Transform.h"
#include "../Weapon/WeaponPunch.h"
#include "../EnemyCommon.h"
#include "Statue.h"

Statue::Statue(void)
{
}

Statue::~Statue(void)
{
	if (useWeapon_)
	{
		delete useWeapon_;
		useWeapon_ = nullptr;
	}
}

void Statue::Init(void)
{
	EnemyBase::Init();

	const auto& data = EnemyTable::Table.at(ENEMY_TAG::STATUE);
	SetEnemyData(data);

	// Transform座標の設定
	if (transform_)
	{
		if (transform_->pos_.y <= 0.0f)
		{
			transform_->pos_ = DEFAULT_POS;
		}
	}

	if (minAreaPos_.x == -1.0f && maxAreaPos_.x == -1.0f)
	{
		minAreaPos_ = MIN_AREA_POS;
		maxAreaPos_ = MAX_AREA_POS;
	}

	if (chasePos_.y <= 0.0f)
	{
		chasePos_ = DEFAULT_POS;
	}

	// パラメータ初期化
	info_.moveDir_ = Math::VECTOR_ZERO;
	info_.attackMoveSpeed_ = 20.0f;
	info_.attackJumpPow_ = 25.0f;
	info_.attackDamagePow_ = 20.0f;

	seTimer_ = 0.0f;

	useWeapon_ = new WeaponPunch();
	useWeapon_->Init(WeaponBase::TYPE::PUNCH);

	const auto* wayList = pathData_->GetWayList();
	if (wayList && !wayList->empty() && transform_)
	{
		info_.candidates_.reserve(wayList->size());
		info_.currentNodeId_ = FindNearestNode(transform_->pos_);
	}

	ChangeState(STATE::IDLE);
}

void Statue::Update(void)
{
	// 遅延回転処理
	DelayRotate();

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

void Statue::Draw3D(void)
{
	EnemyBase::Draw3D();

#ifdef _DEBUG
	DrawCube3D(minAreaPos_, maxAreaPos_, 0xff00ff, 0xff00ff, false);
	
	// StagePathDataからリストを取得
	const auto* wayList = pathData_->GetWayList();
	const auto* edgeList = pathData_->GetEdgeList();
	if (wayList && edgeList && transform_)
	{
		for (int i = 0; i < (int)edgeList->size(); i++)
		{
			for (const auto& edge : (*edgeList)[i])
			{
				DrawLine3D((*wayList)[i].pos, edge.way.pos, GetColor(255, 255, 0));
			}
		}

		if (info_.currentNodeId_ >= 0 && info_.currentNodeId_ < (int)wayList->size())
		{
			DrawLine3D(transform_->pos_, (*wayList)[info_.currentNodeId_].pos, GetColor(255, 0, 255));
		}

		DrawSphere3D(transform_->pos_, info_.patrolRadius_, 8, GetColor(0, 255, 0), GetColor(0, 0, 0), FALSE);
		DrawSphere3D(info_.nextWayPoint_, 40.0f, 10, GetColor(0, 255, 0), GetColor(0, 255, 0), TRUE);
		DrawLine3D(transform_->pos_, info_.nextWayPoint_, GetColor(0, 255, 0));
	}

	if (useWeapon_)
	{
		useWeapon_->Draw();
	}
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
	info_.step_ = 4.0f;
}

void Statue::ChangeChase(void)
{
	info_.moveSpeed_ = 5.0f;
}

void Statue::ChangeAttack(void)
{
	info_.step_ = 10.0f;
	LookPlayer();
	VECTOR enemyAttackPos = transform_->pos_;
	enemyAttackPos.y += 80.0f;
	useWeapon_->Use(enemyAttackPos, info_.moveDir_);
}

void Statue::ChangeEnd(void)
{
}

void Statue::UpdateIdle(void)
{
	if (IsPlayerInArea(minAreaPos_, maxAreaPos_))
	{
		ChangeState(STATE::SURPRISE);
		return;
	}
}

void Statue::UpdateSurprise(void)
{
	info_.step_ -= SceneManager::GetInstance()->GetDeltaTime();
	if (info_.step_ < 0.0f)
	{
		// 待機終了
		ChangeState(STATE::CHASE);
		return;
	}
}

void Statue::UpdateChase(void)
{
	VECTOR enemyPos = transform_->pos_;
	VECTOR playerPos = player_->GetTransform()->pos_;

	// 視線位置
	VECTOR start = { 0.0f, 100.0f, 0.0f };
	VECTOR enemyHead = VAdd(transform_->pos_, start);
	VECTOR playerHead = player_->GetCapsule()->GetStart();
	float distance = VSize(VSub(playerPos, transform_->pos_));

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
	enemyPos.y -= 350.0f;

	bool isLooked = (isLookedByPlayer1 || isLookedByPlayer2 || isLookedByPlayer3 || isLookedByPlayer4 || isLookedByPlayer5);

	// 視線チェック
	bool isPlayerVisible = !CheckChaseLineCollision(enemyHead, playerHead, 40.0f);

	// プレイヤーがエリア内にいるなら
	if (IsPlayerInArea(minAreaPos_, maxAreaPos_))
	{
		//　攻撃範囲内にいたら、攻撃状態にする
		if (distance <= 200.0f)
		{
			ChangeState(STATE::ATTACK);
			return;
		}

		// プレイヤーに見られているなら
		if (isLooked)
		{
			info_.path_.clear();
			return;
		}
		else if (!isPlayerVisible)
		{
			if (info_.path_.empty())
			{
				// プレイヤーから一番近いノードを探す
				int playerNearNodeId = FindNearestNode(playerPos);
				// 敵から一番近いノードを探す
				int enemyNearNodeId = FindNearestNode(enemyPos);
				// 敵の位置とプレイヤーの位置を繋ぐルートを探す
				FindPath(enemyNearNodeId, playerNearNodeId);

				if (info_.path_.size() > 1)
				{
					info_.nextNodeId_ = 1;
				}
				else
				{
					info_.nextNodeId_ = 0;
				}
			}
			else
			{
				if (info_.nextNodeId_ >= static_cast<int>(info_.path_.size()))
				{
					info_.path_.clear();
					info_.nextNodeId_ = 0;
				}
				else
				{
					ChaseNode();
				}
			}
		}
		else
		{
			info_.path_.clear();
			ChaseDirect();

		}

	}
	else
	{
		// 見られていないなら帰還開始
		if (CheckCameraViewClip(enemyPos))
		{

			// 帰還先との間に障害物があるかチェック
			if (CheckChaseLineCollision(enemyPos, chasePos_, 30.0f))
			{
				// 障害物あり
				if (info_.path_.empty())
				{
					int enemyNearNode = FindNearestNode(enemyPos);
					int defaultNearNode = FindNearestNode(chasePos_);
					FindPath(enemyNearNode, defaultNearNode);
					if (info_.path_.size() > 1)
					{
						info_.nextNodeId_ = 1;
					}
					else
					{
						info_.nextNodeId_ = 0;
					}
				}

				if (info_.nextNodeId_ >= static_cast<int>(info_.path_.size())) 
				{
					info_.path_.clear();
					info_.nextNodeId_ = 0;
				}
				else
				{
					ChaseNode();
				}
			}
			else
			{
				// 相手へのベクトルを計算
				VECTOR diff = VSub(chasePos_, enemyPos);
				diff.y = 0.0f;

				// ベクトルの正規化で単位ベクトル（方向）を取得
				info_.moveDir_ = VNorm(diff);

				// 回転はY軸のみ
				transform_->angle_.x = transform_->angle_.z = 0.0f;

				float enemyDist2 = GetDistance(enemyPos, chasePos_);

				if (enemyDist2 <= 100.0f * 100.0f)
				{
					ChangeState(STATE::IDLE);
					return;
				}
			}
		}
		else
		{

			info_.path_.clear();

			if (isPlayerVisible)
			{
				info_.moveDir_ = { 0.0f, 0.0f, 0.0f };
			}
			// 帰還先との間に障害物があるかチェック
			else if (CheckChaseLineCollision(enemyPos, chasePos_, 40.0f))
			{
				// 障害物あり
				if (info_.path_.empty())
				{
					int enemyNearNode = FindNearestNode(enemyPos);
					int defaultNearNode = FindNearestNode(chasePos_);
					FindPath(enemyNearNode, defaultNearNode);
					if (info_.path_.size() > 1)
					{
						info_.nextNodeId_ = 1;
					}
					else
					{
						info_.nextNodeId_ = 0;
					}
				}

				if (info_.nextNodeId_ >= static_cast<int>(info_.path_.size()))
				{
					info_.path_.clear();
					info_.nextNodeId_ = 0;
				}
				else
				{
					ChaseNode();
				}
			}
			else
			{
				// 相手へのベクトルを計算
				VECTOR diff = VSub(chasePos_, enemyPos);
				diff.y = 0.0f;

				// ベクトルの正規化で単位ベクトル（方向）を取得
				info_.moveDir_ = VNorm(diff);

				// 回転はY軸のみ
				transform_->angle_.x = transform_->angle_.z = 0.0f;

				float enemyDist2 = GetDistance(enemyPos, chasePos_);

				if (enemyDist2 <= 100.0f * 100.0f)
				{
					ChangeState(STATE::IDLE);
					return;
				}
			}
		}
	}

	Move();

	// 重力処理
	ApplyGravity();

	// 移動しているか
	bool isMoving = (VSize(info_.moveDir_) > 0.001f);

	if (!isLooked && isMoving)
	{
		seTimer_ -= SceneManager::GetInstance()->GetDeltaTime();
		if (seTimer_ <= 0.0f)
		{
			AudioManager::GetInstance()->PlaySE(SoundID::SE_ENEMY_STATUE, &transform_->pos_,3500.0f);
			seTimer_ = 1.0f;
		}
	}
	else
	{
		seTimer_ = 0.0f;
	}

}

void Statue::UpdateAttack(void)
{
	info_.step_ -= SceneManager::GetInstance()->GetDeltaTime();

	// 攻撃処理の更新
	useWeapon_->Update();


	if (info_.step_ < 0.0f)
	{
		useWeapon_->SetAlive(false);
		ChangeState(STATE::CHASE);
		return;
	}
}

void Statue::UpdateEnd(void)
{
}