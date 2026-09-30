#include "../../../../Scene/SceneManager.h"
#include "../../../../Manager/Audio/AudioManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../../../Common/Transform/MatrixUtility.h"
#include "../../PlayerController/PlayerController.h"
#include "../../Collider/3DCollider/CapsuleCollider.h"
#include "../../Collider/StageCollider/StageCollider.h"
#include "../../Transform/Transform.h"
#include "../Weapon/WeaponPunch.h"
#include "../EnemyCommon.h"

#include "Statue.h"

Statue::Statue(void)
{
}

Statue::~Statue(void)
{
	// 武器が生成されている場合のみ後始末を行う
	if (useWeapon_)
	{
		useWeapon_->Release();
		delete useWeapon_;
		useWeapon_ = nullptr;
	}
}

void Statue::Init(void)
{
	EnemyBase::Init();

	// 敵のデータを取得して設定
	const auto& data = EnemyTable::Table.at(ENEMY_TAG::STATUE);
	SetEnemyData(data);

	// パラメータ初期化
	// 移動方向を0にしておく
	info_.moveDir_ = Math::VECTOR_ZERO;

	// 攻撃時移動速度
	info_.attackMoveSpeed_ = ATTACK_SPEED_MOVE;

	// 攻撃時吹っ飛び力
	info_.attackJumpPow_ = ATTACK_JUMP_POWER;

	// 攻撃時ダメージ力
	info_.attackDamagePow_ = ATTACK_DAMAGE_POWER;

	// 敵のタグを設定
	info_.tag_ = ENEMY_TAG::STATUE;

	if (transform_)
	{
		// モデルの大きさを設定
		info_.scale_ = SCALE;
		MV1SetScale(info_.modelId_, info_.scale_);

		// 敵の向きを設定
		transform_->angle_ = DEFAULT_ANGLE;
		info_.localAngle_ = { 0.0f, Math::Deg2Rad(180.0f), 0.0f };

		// 前回の座標を現在の座標に設定
		transform_->prevPos_ = transform_->pos_;
	}

	// 足音SEのタイマーを初期化
	seTimer_ = 0.0f;

	// 武器が未生成なら生成して初期化する
	if (useWeapon_ == nullptr)
	{
		useWeapon_ = new WeaponPunch();
		useWeapon_->Init(WeaponBase::TYPE::PUNCH);
	}

	// 初期ステートの設定
	ChangeState(STATE::IDLE);

	// lock() して shared_ptr を一時的に取得
	auto pathData = pathData_.lock();
	if (!pathData) return;

	// ウェイポイント（巡回ノード）のリストを取得
	const auto* wayList = pathData->GetWayList();
	if (wayList && !wayList->empty() && transform_)
	{
		// 候補ノード用の領域を事前確保して再確保を防ぐ
		info_.candidates_.reserve(wayList->size());

		// 現在位置から一番近いノードを現在ノードとして登録
		info_.currentNodeId_ = FindNearestNode(transform_->pos_);
	}
}

void Statue::Update(void)
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

	// 現在のステートに応じた更新処理を呼び分ける
	switch (state_)
	{
	case Statue::STATE::IDLE: 
		UpdateIdle(); 
		break;

	case Statue::STATE::SURPRISE:
		UpdateSurprise();
		break;

	case Statue::STATE::CHASE: 
		UpdateChase(); 
		break;

	case Statue::STATE::ATTACK: 
		UpdateAttack(); 
		break;

	default:
		break;
	}

	// ステージとの衝突判定・押し出し計算
	if (stageColl_)
	{
		stageColl_->StageColl(info_.velocityY_);
	}

	// モデルの更新
	MV1RefreshCollInfo(info_.modelId_, -1);
}

void Statue::Draw3D(void)
{
	EnemyBase::Draw3D();

#ifdef _DEBUG
	// プレイヤーを検知する行動エリアをマゼンタのワイヤーフレームで描画
	DrawCube3D(minAreaPos_, maxAreaPos_, 0xff00ff, 0xff00ff, false);
	
	// lock() して shared_ptr を一時的に取得
	auto pathData = pathData_.lock();
	if (!pathData) return;

	// StagePathDataからリストを取得
	const auto* wayList = pathData->GetWayList();
	const auto* edgeList = pathData->GetEdgeList();
	if (wayList && edgeList && transform_)
	{
		// すべてのノード間の接続を黄色い線で描画
		for (int i = 0; i < (int)edgeList->size(); i++)
		{
			for (const auto& edge : (*edgeList)[i])
			{
				DrawLine3D((*wayList)[i].pos, 
							edge.way.pos, 
							GetColor(255, 255, 0));
			}
		}

		// 現在ノードと自分の位置をマゼンタの線で結ぶ
		if (info_.currentNodeId_ >= 0 && info_.currentNodeId_ < (int)wayList->size())
		{
			DrawLine3D(transform_->pos_, 
						(*wayList)[info_.currentNodeId_].pos, 
						GetColor(255, 0, 255));
		}

		// 巡回範囲を緑のワイヤーフレーム球で描画
		DrawSphere3D(transform_->pos_, 
					info_.patrolRadius_, 
					8, GetColor(0, 255, 0),
					GetColor(0, 0, 0), 
					FALSE);

		// 次の目的地を緑の塗りつぶし球と線で描画
		DrawSphere3D(info_.nextWayPoint_, 
					40.0f, 
					10, 
					GetColor(0, 255, 0), 
					GetColor(0, 255, 0), 
					TRUE);

		DrawLine3D(transform_->pos_, 
					info_.nextWayPoint_, 
					GetColor(0, 255, 0));
	}

	// 武器の当たり判定などをデバッグ描画
	if (useWeapon_)
	{
		useWeapon_->Draw();
	}
#endif
}

void Statue::SetAreaPos(VECTOR minPos, VECTOR maxPos) 
{ 
	minAreaPos_ = minPos; 
	maxAreaPos_ = maxPos; 
}

void Statue::SetChasePos(VECTOR pos) 
{ 
	chasePos_ = pos; 
}

void Statue::ChangeState(STATE state)
{
	state_ = state;

	// 遷移時に一度だけ行う初期化処理を呼び出す
	switch (state_)
	{
	case Statue::STATE::IDLE: 
		ChangeIdle(); 
		break;

	case Statue::STATE::SURPRISE: 
		ChangeSurprise(); 
		break;

	case Statue::STATE::CHASE: 
		ChangeChase(); 
		break;

	case Statue::STATE::ATTACK: 
		ChangeAttack(); 
		break;

	default:
		break;
	}
}

void Statue::ChangeIdle(void)
{
}

void Statue::ChangeSurprise(void)
{
	// 驚いている時間（猶予時間）を設定
	info_.step_ = STEP_TIME_SURPRISE;
}

void Statue::ChangeChase(void)
{
	// 追跡時の移動速度を設定
	info_.moveSpeed_ = SPEED_CHASE;
}

void Statue::ChangeAttack(void)
{
	// 攻撃を続ける時間を設定
	info_.step_ = STEP_TIME_ATTACK;

	// プレイヤーの方を向く
	LookPlayer();

	// 敵の位置
	VECTOR enemyAttackPos = transform_->pos_;

	// 攻撃の発生位置は、足元から一定の高さ分だけ上げた位置にする
	enemyAttackPos.y += ATTACK_POS_OFFSET_Y;

	// パンチ攻撃を発動
	useWeapon_->Use(enemyAttackPos, info_.moveDir_);
}

void Statue::UpdateIdle(void)
{
	// プレイヤーが行動エリア内に入ったら発見状態へ
	if (IsPlayerInArea(minAreaPos_, maxAreaPos_))
	{
		ChangeState(STATE::SURPRISE);
		return;
	}
}

void Statue::UpdateSurprise(void)
{
	// 驚き時間を経過時間分減らす
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
	// 敵とプレイヤーの現在位置
	VECTOR enemyPos = transform_->pos_;
	VECTOR playerPos = player_->GetTransform()->pos_;

	// 視線位置
	VECTOR start = { 0.0f, EYE_OFFSET_Y, 0.0f };
	VECTOR enemyHead = VAdd(transform_->pos_, start);
	VECTOR playerHead = player_->GetCapsule()->GetStart();

	// プレイヤーとの距離
	float distance = VSize(VSub(playerPos, transform_->pos_));

	// 画面内に入っているかをチェックする
	bool isLookedByPlayer1 = !CheckCameraViewClip(enemyPos);
	enemyPos.y += VIEW_CHECK_STEP_Y;
	bool isLookedByPlayer2 = !CheckCameraViewClip(enemyPos);
	enemyPos.y += VIEW_CHECK_STEP_Y;
	bool isLookedByPlayer3 = !CheckCameraViewClip(enemyPos);
	enemyPos.y += VIEW_CHECK_STEP_Y;
	bool isLookedByPlayer4 = !CheckCameraViewClip(enemyPos);
	enemyPos.y += VIEW_CHECK_STEP_Y;
	bool isLookedByPlayer5 = !CheckCameraViewClip(enemyPos);

	// 判定用に上げた高さを戻す
	enemyPos.y -= VIEW_CHECK_RETURN_Y;

	// 5点のうち1点でも見られていれば true
	bool isLooked = (isLookedByPlayer1 || isLookedByPlayer2 || isLookedByPlayer3 || isLookedByPlayer4 || isLookedByPlayer5);

	// 視線チェック
	// 敵の頭からプレイヤーの頭までの間に障害物がなければ見えている
	bool isPlayerVisible = !CheckChaseLineCollision(enemyHead, playerHead, RAY_RADIUS);

	// プレイヤーがエリア内にいるなら
	if (IsPlayerInArea(minAreaPos_, maxAreaPos_))
	{
		//　攻撃範囲内にいたら、攻撃状態にする
		if (distance <= ATTACK_RANGE)
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
			// プレイヤーに見られておらず、障害物がある場合は
			// 経路探索でノードをたどって追う
			if (info_.path_.empty())
			{
				// プレイヤーから一番近いノードを探す
				int playerNearNodeId = FindNearestNode(playerPos);

				// 敵から一番近いノードを探す
				int enemyNearNodeId = FindNearestNode(enemyPos);

				// 敵の位置とプレイヤーの位置を繋ぐルートを探す
				FindPath(enemyNearNodeId, playerNearNodeId);

				// 経路の0番目は自分に最も近いノードなので、
				// 2つ以上あれば1番目から向かう
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
				// 経路を最後までたどり終えたらクリアして次回再探索する
				if (info_.nextNodeId_ >= static_cast<int>(info_.path_.size()))
				{
					info_.path_.clear();
					info_.nextNodeId_ = 0;
				}
				else
				{
					// 経路がまだ残っているなら次のノードへ向かって移動する
					ChaseNode();
				}
			}
		}
		else
		{
			// 見られておらず、プレイヤーが直接見えているなら
			// 経路を使わずプレイヤーへ直進する
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
			if (CheckChaseLineCollision(enemyPos, chasePos_, RAY_RADIUS))
			{
				// 障害物あり
				if (info_.path_.empty())
				{
					// 敵から一番近いノードを探す
					int enemyNearNode = FindNearestNode(enemyPos);

					// 帰還先から一番近いノードを探す
					int defaultNearNode = FindNearestNode(chasePos_);

					// 敵の位置と帰還先を繋ぐルートを探す
					FindPath(enemyNearNode, defaultNearNode);

					// 2つ以上あれば1番目から向かう
					if (info_.path_.size() > 1)
					{
						info_.nextNodeId_ = 1;
					}
					else
					{
						info_.nextNodeId_ = 0;
					}
				}

				// 経路を最後までたどり終えたらクリアする
				if (info_.nextNodeId_ >= static_cast<int>(info_.path_.size())) 
				{
					info_.path_.clear();
					info_.nextNodeId_ = 0;
				}
				else
				{
					// 次のノードへ向かって移動する
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

				// 帰還先までの距離を計算
				float distToChasePos = GetDistanceSQ(enemyPos, chasePos_);

				// 帰還先に十分近づいたら待機状態に戻る
				if (distToChasePos <= RETURN_ARRIVE_DISTANCE)
				{
					ChangeState(STATE::IDLE);
					return;
				}
			}
		}
		else
		{
			// 経路はクリアしておく
			info_.path_.clear();

			if (isPlayerVisible)
			{
				// プレイヤーから直接見えているなら、その場で止まる
				info_.moveDir_ = { 0.0f, 0.0f, 0.0f };
			}
			// 帰還先との間に障害物があるかチェック
			else if (CheckChaseLineCollision(enemyPos, chasePos_, RAY_RADIUS))
			{
				// 障害物あり
				if (info_.path_.empty())
				{
					// 敵から一番近いノードを探す
					int enemyNearNode = FindNearestNode(enemyPos);

					// 帰還先から一番近いノードを探す
					int defaultNearNode = FindNearestNode(chasePos_);

					// 敵の位置とプレイヤーの位置を繋ぐルートを探す
					FindPath(enemyNearNode, defaultNearNode);

					// 2つ以上あれば1番目から向かう
					if (info_.path_.size() > 1)
					{
						info_.nextNodeId_ = 1;
					}
					else
					{
						info_.nextNodeId_ = 0;
					}
				}

				// 経路を最後までたどり終えたらクリアする
				if (info_.nextNodeId_ >= static_cast<int>(info_.path_.size()))
				{
					info_.path_.clear();
					info_.nextNodeId_ = 0;
				}
				else
				{
					// 次のノードへ向かって移動する
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

				// 帰還先までの距離を計算
				float distToChasePos = GetDistanceSQ(enemyPos, chasePos_);

				// 帰還先に十分近づいたら待機状態に戻る
				if (distToChasePos <= RETURN_ARRIVE_DISTANCE)
				{
					ChangeState(STATE::IDLE);
					return;
				}
			}
		}
	}

	// 決定した移動方向に従って移動する
	Move();

	// 重力処理
	ApplyGravity();

	// 移動しているか
	bool isMoving = (VSize(info_.moveDir_) > MOVE_EPSILON);

	// プレイヤーに見られておらず、かつ移動している間だけ足音SEを鳴らす
	if (!isLooked && isMoving)
	{
		seTimer_ -= SceneManager::GetInstance()->GetDeltaTime();

		// 一定間隔で足音SEを鳴らす
		if (seTimer_ <= 0.0f)
		{
			AudioManager::GetInstance()->PlaySE(
							SoundID::SE_ENEMY_STATUE, 
							&transform_->pos_,
							SE_RADIUS_STATUE);

			seTimer_ = SE_INTERVAL_CHASE;
		}
	}
	else
	{
		// 止まっているまたは、見られている間はタイマーをリセット
		seTimer_ = 0.0f;
	}
}

void Statue::UpdateAttack(void)
{
	// 攻撃時間を経過時間分減らす
	info_.step_ -= SceneManager::GetInstance()->GetDeltaTime();

	// 攻撃処理の更新
	useWeapon_->Update();

	// 攻撃時間が終わったら
	if (info_.step_ < 0.0f)
	{
		// 武器の判定を無効化して、追跡状態に戻る
		useWeapon_->SetAlive(false);
		ChangeState(STATE::CHASE);
		return;
	}
}