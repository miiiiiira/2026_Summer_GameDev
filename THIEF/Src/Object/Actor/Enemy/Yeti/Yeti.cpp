#include "../../../../Application.h"
#include "../../../../Scene/SceneManager.h"
#include "../../../../Manager/Audio/AudioManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../../../Common/Transform/MatrixUtility.h"
#include "../../../Component/PlayerController/PlayerController.h"
#include "../../../Component/Collider/3DCollider/CapsuleCollider.h"
#include "../../../Component/Collider/StageCollider/StageCollider.h"
#include "../../../Component/Transform/Transform.h"
#include "../../../Component/Animation/Animation.h"
#include "../Weapon/WeaponPunch.h"
#include "../../../../Common/Shader/Shader.h"
#include "../EnemyCommon.h"
#include "Yeti.h"


Yeti::Yeti(void)
{
}

Yeti::~Yeti(void)
{
	if (useWeapon_)
	{
		useWeapon_->Release();
		delete useWeapon_;
		useWeapon_ = nullptr;
	}
}

void Yeti::Init(void)
{
	EnemyBase::Init();

	anim_->Init();
	for(int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
	{
		anim_->AddInFbx(static_cast<int>(i), 0.2f, static_cast<int>(i));
	}

	const auto& data = EnemyTable::Table.at(ENEMY_TAG::YETI);
	SetEnemyData(data);

	if (transform_)
	{
		info_.scale_ = SCALE;

		transform_->angle_ = DEFAULT_ANGLE;
		info_.localAngle_ = { 0.0f, Math::Deg2Rad(180.0f), 0.0f };

		MATRIX mat = Matrix::Multiplication(info_.localAngle_, transform_->angle_);

		transform_->prevPos_ = transform_->pos_;
	}

	info_.moveDir_ = Math::VECTOR_ZERO;
	info_.moveSpeed_ = 0.0f;

	info_.patrolRadius_ = 1500.0f;
	info_.viewRadius_ = 1000.0f;

	info_.attackMoveSpeed_ = 30.0f;
	info_.attackJumpPow_ = 25.0f;
	info_.attackDamagePow_ = 20.0f;

	seTimer_ = 0.0f;


	if (useWeapon_)
	{
		useWeapon_->Release();
		delete useWeapon_;
		useWeapon_ = nullptr;
	}

	useWeapon_ = new WeaponPunch();
	useWeapon_->Init(WeaponBase::TYPE::PUNCH);

	// 初期アニメーション再生
	anim_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);

	const auto* wayList = pathData_->GetWayList();
	if (wayList && !wayList->empty() && transform_)
	{
		info_.candidates_.reserve(wayList->size());
		info_.currentNodeId_ = FindNearestNode(transform_->pos_);
	}

	ChangeState(STATE::IDLE);
}

void Yeti::Update(void)
{
	transform_->prevPos_ = transform_->pos_;

	// 遅延回転処理
	DelayRotate();

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
}

void Yeti::Draw3D(void)
{
	EnemyBase::Draw3D();

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
	
#ifdef _DEBUG
	if (pathData_ && transform_)
	{
		const auto* wayList = pathData_->GetWayList();
		const auto* edgeList = pathData_->GetEdgeList();

		if (wayList && edgeList)
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

			for (const auto& point : (*wayList))
			{
				float distance = VSize(VSub(point.pos, transform_->pos_));

				unsigned int color = 0x0000ff;
				if (point.id == info_.prevNodeId_)
				{
					color = 0xff8c00;
				}
				else if (point.id == info_.prevPrevNodeId_)
				{
					color = 0xfff5ee;
				}
				else if (distance > info_.patrolRadius_)
				{
					color = 0xff0000;
				}

				DrawSphere3D(
					point.pos, 50.0f, 10,
					color, color, false);
			}
		}
	}

	if (useWeapon_)
	{
		useWeapon_->Draw();
	}
#endif
}

void Yeti::Draw2D(void)
{
	DrawFormatString(10, 200, GetColor(255, 255, 255), "Yetiの座標: %.2f, %.2f, %.2f",transform_->pos_.x, transform_->pos_.y, transform_->pos_.z);
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
	info_.step_ = 5.0f;
	anim_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void Yeti::ChangePatrol(void)
{
	// 移動量ゼロ
	info_.movePow_ = Math::VECTOR_ZERO;

	int lastNodeId = info_.currentNodeId_;
	
	ArriveNode();

	if (info_.currentNodeId_ == lastNodeId)
	{
		ChangeState(STATE::IDLE);
		return;
	}

	// 移動方向を設定
	SetMoveDirPatrol();

	// 移動スピード
	info_.moveSpeed_ = 5.0f;

	anim_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
}

void Yeti::ChangeSurprise(void)
{
	info_.step_ = 2.0f;
	LookPlayer();
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ENEMY_YETI, & transform_->pos_,2000.0f);
	anim_->Play(static_cast<int>(ANIM_TYPE::HIT_REACT), false);
}

void Yeti::ChangeChase(void)
{
	info_.moveSpeed_ = 10.0f;
	chaseTimer_ = 0.0f;
	info_.targetLostTimer_ = 0.0f;
	info_.isNotice_ = false;
	anim_->Play(static_cast<int>(ANIM_TYPE::RUN), true);
}

void Yeti::ChangeAttack(void)
{
	LookPlayer();
	useWeapon_->Use(transform_->pos_, info_.moveDir_);
	anim_->Play(static_cast<int>(ANIM_TYPE::PUNCH), false);
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
	// 10%で待機、90%で徘徊
	int rand = GetRand(100);
	if (rand < 10)
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
	if (CheckPlayerDiscovery(info_.viewRadius_))
	{
		ChangeState(STATE::SURPRISE);
		return;
	}

	info_.step_ -= SceneManager::GetInstance()->GetDeltaTime();

	if (info_.step_ < 0.0f)
	{
		// 待機終了
		ChangeState(STATE::THINK);
		return;
	}
}

void Yeti::UpdatePatrol(void)
{
	if (CheckPlayerDiscovery(info_.viewRadius_))
	{
		ChangeState(STATE::SURPRISE);
		return;
	}

	SetMoveDirPatrol();

	Move();

	// 目的地までの距離を測る
	VECTOR target = VSub(info_.nextWayPoint_, transform_->pos_);
	target.y = 0.0f;
	float dist = VSquareSize(target);

	// ある程度近づいたら考えるステートに移る
	if (dist < 50.0f * 50.0f)
	{
		ChangeState(STATE::THINK);
		return;
	}

	// 移動しているか
	bool isMoving = (VSize(info_.moveDir_) > 0.001f);

	if (isMoving)
	{
		seTimer_ -= SceneManager::GetInstance()->GetDeltaTime();
		if (seTimer_ <= 0.0f)
		{
			AudioManager::GetInstance()->PlaySE(SoundID::SE_ENEMY_YETI_MOVE, &transform_->pos_,3500.0f);
			seTimer_ = 0.5f;
		}
	}
	else
	{
		seTimer_ = 0.0f;
	}
}

void Yeti::UpdateSurprise(void)
{
	info_.step_ -= SceneManager::GetInstance()->GetDeltaTime();
	if (info_.step_ < 0.0f)
	{
		// 待機終了
		ChangeState(STATE::CHASE);
		return;
	}
}

void Yeti::UpdateChase(void)
{
	VECTOR enemyPos = transform_->pos_;
	VECTOR playerPos = player_->GetTransform()->pos_;

	// 視線位置
	VECTOR enemyHead = VAdd(transform_->pos_, info_.startOffset_);
	VECTOR playerHead = player_->GetCapsule()->GetStart();
	float distance = VSize(VSub(playerPos, enemyPos));

	// 視線チェック
	bool isPlayerVisible = false;

	// 頭位置の高さの差を計算する
	float heightDiff = fabsf(playerHead.y - enemyHead.y);

	// 高低差が700.0fより大きければ、見えない判定にする
	if (heightDiff > 700.0f || distance > info_.viewRadius_)
	{
		isPlayerVisible = false;
		info_.isHit_ = true;
	}
	else
	{
		// 高低差が範囲内なら、視線チェックする
		isPlayerVisible = !CheckChaseLineCollision(enemyHead, playerHead, 40.0f);
		info_.isHit_ = !isPlayerVisible;
	}

	// プレイヤーを見つけたなら
	if (isPlayerVisible)
	{
		// 見失いタイマーをリセット
		info_.targetLostTimer_ = 0.0f;

		//　攻撃範囲内にいたら、攻撃状態にする
		if (distance <= 300.0f)
		{
			ChangeState(STATE::ATTACK);
			return;
		}
	}
	else
	{
		// 見失いタイマーをカウントさせる
		info_.targetLostTimer_ += SceneManager::GetInstance()->GetDeltaTime();
	}

	// タイマーがリミットより多くなったら、近くのノードを探して
	// そこから巡回をさせる
	if (info_.targetLostTimer_ >= LOST_LIMIT_TIME)
	{
		info_.path_.clear();
		info_.nextNodeId_ = 0;

		info_.currentNodeId_ = FindNearestNode(transform_->pos_);
		info_.prevNodeId_ = -1;
		info_.prevPrevNodeId_ = -1;

		const auto* wayList = pathData_->GetWayList();
		info_.nextWayPoint_ = (*wayList)[info_.currentNodeId_].pos;

		ChangeState(STATE::IDLE);
		return;
	}

	// タイマーを進める
	chaseTimer_ += SceneManager::GetInstance()->GetDeltaTime();

	if (chaseTimer_ >= CHASE_INTERVAL)
	{
		if (info_.isHit_ && info_.path_.empty())
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

		// 判定が終わったらタイマーをリセットする
		chaseTimer_ = 0.0f;
	}

	if (info_.path_.empty())
	{
		// ルートがないなら、直接追従
		ChaseDirect();
	}
	else
	{
		// 範囲外チェック
		// 最後のノードにたどり着いたかチェックする
		if (info_.nextNodeId_ >= static_cast<int>(info_.path_.size()))
		{
			info_.path_.clear();
			info_.nextNodeId_ = 0;

			if (!isPlayerVisible)
			{
				// プレイヤーの現在の位置に一番近いノードを取得
				int playerNearNodeId = FindNearestNode(playerPos);

				// 自分が今いる位置が、そのノードの近くであるか判定
				const auto* wayList = pathData_->GetWayList();
				float distance = VSize(VSub(transform_->pos_, (*wayList)[playerNearNodeId].pos));

				// プレイヤーを見失っているかつ、
				// プレイヤーに一番近いノードまで近づいているなら
				if (distance < 100.0f)
				{
					info_.currentNodeId_ = FindNearestNode(transform_->pos_);
					info_.prevNodeId_ = -1;
					info_.prevPrevNodeId_ = -1;
					info_.nextWayPoint_ = (*wayList)[info_.currentNodeId_].pos;
					LookPlayer();

					ChangeState(STATE::IDLE);
					return;
				}
			}
		}
		else
		{
			// ルートがあるなら、次のノードがあるか確認してからルート追従
			ChaseNode();
		}
	}
	Move();

	// 移動しているか
	bool isMoving = (VSize(info_.moveDir_) > 0.001f);

	if (isMoving)
	{
		seTimer_ -= SceneManager::GetInstance()->GetDeltaTime();
		if (seTimer_ <= 0.0f)
		{
			AudioManager::GetInstance()->PlaySE(SoundID::SE_ENEMY_YETI_MOVE, &transform_->pos_, 3500.0f);
			seTimer_ = 0.3f;
		}
	}
	else
	{
		seTimer_ = 0.0f;
	}
}

void Yeti::UpdateAttack(void)
{
	// 攻撃処理の更新
	useWeapon_->Update();

	if (anim_->IsEnd())
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
