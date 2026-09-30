#include "../../../../Application.h"
#include "../../../../Scene/SceneManager.h"
#include "../../../../Manager/Audio/AudioManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../../../Common/Transform/MatrixUtility.h"
#include "../../PlayerController/PlayerController.h"
#include "../../Collider/3DCollider/CapsuleCollider.h"
#include "../../Collider/StageCollider/StageCollider.h"
#include "../../Transform/Transform.h"
#include "../../Animation/Animation.h"
#include "../Weapon/WeaponPunch.h"
#include "../EnemyCommon.h"

#include "Yeti.h"

Yeti::Yeti(void)
{
}

Yeti::~Yeti(void)
{
	// 武器が生成されている場合のみ後始末を行う
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

	// アニメーションの初期化
	animation_->Init();

	// アニメーションの追加
	for(int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
	{
		animation_->AddInFbx(
					static_cast<int>(i), 
					ANIM_SPEED,
					static_cast<int>(i));
	}

	// 敵のデータを取得して設定
	const auto& data = EnemyTable::Table.at(ENEMY_TAG::YETI);
	SetEnemyData(data);

	// パラメータ初期化
	// 移動方向・移動速度を 0 にしておく
	info_.moveDir_ = Math::VECTOR_ZERO;
	info_.moveSpeed_ = 0.0f;

	// 巡回半径と視認半径の初期値
	info_.patrolRadius_ = PATROL_RADIUS_DEFAULT;
	info_.viewRadius_ = VIEW_RADIUS_DEFAULT;

	// 攻撃時移動速度
	info_.attackMoveSpeed_ = ATTACK_SPEED_MOVE;

	// 攻撃時吹っ飛び力
	info_.attackJumpPow_ = ATTACK_JUMP_POWER;

	// 攻撃時ダメージ力
	info_.attackDamagePow_ = ATTACK_DAMAGE_POWER;

	// 敵のタグを設定
	info_.tag_ = ENEMY_TAG::YETI;

	// 足音SEのタイマーを初期化
	seTimer_ = 0.0f;

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

	// 武器が未生成なら生成して初期化する
	if (useWeapon_ == nullptr)
	{
		useWeapon_ = new WeaponPunch();
		useWeapon_->Init(WeaponBase::TYPE::PUNCH);
	}

	// 初期アニメーション再生
	animation_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);

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

void Yeti::Update(void)
{
	// 前フレームの座標を保存
	transform_->prevPos_ = transform_->pos_;

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
	case Yeti::STATE::THINK: 
		UpdateThink();
		break;

	case Yeti::STATE::IDLE: 
		UpdateIdle(); 
		break;

	case Yeti::STATE::PATROL: 
		UpdatePatrol(); 
		break;

	case Yeti::STATE::SURPRISE: 
		UpdateSurprise(); 
		break;

	case Yeti::STATE::CHASE: 
		UpdateChase(); 
		break;

	case Yeti::STATE::ATTACK: 
		UpdateAttack(); 
		break;

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
	if (animation_)
	{
		animation_->Update();
	}

	// モデルの更新
	MV1RefreshCollInfo(info_.modelId_, -1);
}

void Yeti::Draw3D(void)
{
	EnemyBase::Draw3D();
	
#ifdef _DEBUG

	// lock() して shared_ptr を一時的に取得
	auto pathData = pathData_.lock();
	if (!pathData) return;

	if (transform_)
	{
		// ウェイポイントと、ノード同士をつなぐ辺（エッジ）のリストを取得
		const auto* wayList = pathData->GetWayList();
		const auto* edgeList = pathData->GetEdgeList();

		if (wayList && edgeList)
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
						8, 
						GetColor(0, 255, 0), 
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

			// 各ノードを状態に応じた色で描画
			for (const auto& point : (*wayList))
			{
				// 自分とノードの距離
				float distance = VSize(VSub(point.pos, transform_->pos_));

				// 基本色は青
				unsigned int color = 0x0000ff;
				if (point.id == info_.prevNodeId_)
				{
					// 直前に訪れたノード：オレンジ
					color = 0xff8c00;
				}
				else if (point.id == info_.prevPrevNodeId_)
				{
					// 2つ前に訪れたノード：白系
					color = 0xfff5ee;
				}
				else if (distance > info_.patrolRadius_)
				{
					// 巡回範囲外のノード：赤
					color = 0xff0000;
				}

				DrawSphere3D(
					point.pos, 50.0f, 10,
					color, color, false);
			}
		}
	}

	// 武器の当たり判定などをデバッグ描画
	if (useWeapon_)
	{
		useWeapon_->Draw();
	}
#endif
}

void Yeti::Draw2D(void)
{
}

void Yeti::ChangeState(STATE state)
{
	state_ = state;

	// 遷移時に一度だけ行う初期化処理を呼び出す
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

	case Yeti::STATE::SURPRISE:
		ChangeSurprise(); 
		break;

	case Yeti::STATE::CHASE: 
		ChangeChase(); 
		break;

	case Yeti::STATE::ATTACK:
		ChangeAttack(); 
		break;

	default:
		break;
	}
}

void Yeti::ChangeThink(void)
{
}

void Yeti::ChangeIdle(void)
{
	// 待機する時間を設定
	info_.step_ = STEP_TIME_IDLE;

	// 待機アニメーションをループ再生
	animation_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void Yeti::ChangePatrol(void)
{
	// 移動量ゼロ
	info_.movePow_ = Math::VECTOR_ZERO;

	// 遷移前のノードIDを記録しておく
	int lastNodeId = info_.currentNodeId_;
	
	// 次に向かうノードを決定して現在ノードを更新する
	ArriveNode();

	// ノードが変わらなかった場合は待機に戻る
	if (info_.currentNodeId_ == lastNodeId)
	{
		ChangeState(STATE::IDLE);
		return;
	}

	// 移動方向を設定
	SetMoveDirPatrol();

	// 移動スピード
	info_.moveSpeed_ = SPEED_PATROL;

	// 歩行アニメーションをループ再生
	animation_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
}

void Yeti::ChangeSurprise(void)
{
	// 驚いている時間（猶予時間）を設定
	info_.step_ = STEP_TIME_SURPRISE;

	// プレイヤーの方を向く
	LookPlayer();

	// 発見時の鳴き声SEを再生
	AudioManager::GetInstance()->PlaySE(
					SoundID::SE_ENEMY_YETI, 
					&transform_->pos_, 
					SE_RADIUS_YETI);

	// 驚きのアニメーションを1回だけ再生
	animation_->Play(static_cast<int>(ANIM_TYPE::HIT_REACT), false);
}

void Yeti::ChangeChase(void)
{
	// 追跡時の移動速度
	info_.moveSpeed_ = SPEED_CHASE;

	// 経路再探索用のタイマーをリセット
	chaseTimer_ = 0.0f;

	// プレイヤーを見失っている時間をリセット
	info_.targetLostTimer_ = 0.0f;

	// 気づき状態を解除
	info_.isNotice_ = false;

	// 走りアニメーションをループ再生
	animation_->Play(static_cast<int>(ANIM_TYPE::RUN), true);
}

void Yeti::ChangeAttack(void)
{
	// プレイヤーの方を向く
	LookPlayer();

	// パンチ攻撃を発動
	useWeapon_->Use(transform_->pos_, info_.moveDir_);

	// パンチアニメーションを1回だけ再生
	animation_->Play(static_cast<int>(ANIM_TYPE::PUNCH), false);
}

void Yeti::UpdateThink(void)
{
	// 思考
	// ランダムに次の行動を決定	
	// 10%で待機、90%で徘徊
	int randomVal = GetRand(MAX_PERCENTAGE);

	if (randomVal < IDLE_PROBABILITY)
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
	// 待機中でもプレイヤーが視界に入ったら発見状態へ
	if (CheckPlayerDiscovery(info_.viewRadius_))
	{
		ChangeState(STATE::SURPRISE);
		return;
	}

	// 待機時間を経過時間分減らす
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
	// プレイヤーを見つけたら
	if (CheckPlayerDiscovery(info_.viewRadius_))
	{
		// 発見状態に遷移
		ChangeState(STATE::SURPRISE);
		return;
	}

	// 目的地に向かう移動方向を更新
	SetMoveDirPatrol();

	// 実際の移動処理
	Move();

	// 目的地までの距離を測る
	// 高さは無視して水平距離のみで判定する
	VECTOR target = VSub(info_.nextWayPoint_, transform_->pos_);
	target.y = 0.0f;
	float dist = VSquareSize(target);

	// ある程度近づいたら考えるステートに移る
	if (dist < WAYPOINT_ARRIVE_DIST * WAYPOINT_ARRIVE_DIST)
	{
		ChangeState(STATE::THINK);
		return;
	}

	// 移動しているか
	bool isMoving = (VSize(info_.moveDir_) > 0.001f);

	if (isMoving)
	{
		// 一定間隔で足音SEを鳴らす
		seTimer_ -= SceneManager::GetInstance()->GetDeltaTime();

		if (seTimer_ <= 0.0f)
		{
			AudioManager::GetInstance()->PlaySE(
							SoundID::SE_ENEMY_YETI_MOVE, 
							&transform_->pos_, 
							SE_RADIUS_MOVE);

			seTimer_ = SE_INTERVAL_PATROL;
		}
	}
	else
	{
		// 止まっている間はタイマーをリセット
		seTimer_ = 0.0f;
	}
}

void Yeti::UpdateSurprise(void)
{
	// 驚き時間を経過時間分減らす
	info_.step_ -= SceneManager::GetInstance()->GetDeltaTime();

	if (info_.step_ < 0.0f)
	{
		// 猶予時間が終わったら追跡状態へ
		ChangeState(STATE::CHASE);
		return;
	}
}

void Yeti::UpdateChase(void)
{
	// 敵とプレイヤーの現在位置
	VECTOR enemyPos = transform_->pos_;
	VECTOR playerPos = player_->GetTransform()->pos_;

	// 視線位置
	// 敵の頭の位置と、プレイヤーのカプセル上端を使う
	VECTOR enemyHead = VAdd(transform_->pos_, info_.startOffset_);
	VECTOR playerHead = player_->GetCapsule()->GetStart();

	// プレイヤーとの距離
	float distance = VSize(VSub(playerPos, enemyPos));

	// 視線チェック
	bool isPlayerVisible = false;

	// 頭位置の高さの差を計算する
	float heightDiff = fabsf(playerHead.y - enemyHead.y);

	// 高低差がHEIGHT_DIFF_LIMITより大きければ、見えない判定にする
	if (heightDiff > HEIGHT_DIFF_LIMIT || distance > info_.viewRadius_)
	{
		isPlayerVisible = false;
		info_.isHit_ = true;
	}
	else
	{
		// 高低差が範囲内なら、視線チェックする
		isPlayerVisible = !CheckChaseLineCollision(
								enemyHead, 
								playerHead,
								RAY_RADIUS_CHASE);

		// 敵の頭とプレイヤーの頭の間に障害物がなければ見えている
		info_.isHit_ = !isPlayerVisible;
	}

	// プレイヤーを見つけたなら
	if (isPlayerVisible)
	{
		// 見失いタイマーをリセット
		info_.targetLostTimer_ = 0.0f;

		//　攻撃範囲内にいたら、攻撃状態にする
		if (distance <= ATTACK_RANGE)
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
		// 追跡用の経路をクリア
		info_.path_.clear();
		info_.nextNodeId_ = 0;

		// 現在位置に一番近いノードを現在ノードにし、履歴をリセット
		info_.currentNodeId_ = FindNearestNode(transform_->pos_);
		info_.prevNodeId_ = -1;
		info_.prevPrevNodeId_ = -1;

		// 待機状態に戻る
		ChangeState(STATE::IDLE);

		// lock() して shared_ptr を一時的に取得
		auto pathData = pathData_.lock();
		if (!pathData) return;

		// 次の目的地を現在ノードの座標に設定
		const auto* wayList = pathData->GetWayList();
		info_.nextWayPoint_ = (*wayList)[info_.currentNodeId_].pos;
		return;
	}

	// タイマーを進める
	chaseTimer_ += SceneManager::GetInstance()->GetDeltaTime();

	// 一定間隔ごとに経路の再探索を行う
	if (chaseTimer_ >= CHASE_INTERVAL)
	{
		// 視線が通っておらず、まだ経路がない場合のみ探索する
		if (info_.isHit_ && info_.path_.empty())
		{
			// プレイヤーから一番近いノードを探す
			int playerNearNodeId = FindNearestNode(playerPos);

			// 敵から一番近いノードを探す
			int enemyNearNodeId = FindNearestNode(enemyPos);

			// 敵の位置とプレイヤーの位置を繋ぐルートを探す
			FindPath(enemyNearNodeId, playerNearNodeId);

			// 経路の0番目は自分に最も近いノードなので、
			// 2つ以上あれば1番目（次のノード）から向かう
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
			// 経路を使い切ったのでクリア
			info_.path_.clear();
			info_.nextNodeId_ = 0;

			if (!isPlayerVisible)
			{
				// プレイヤーの現在の位置に一番近いノードを取得
				int playerNearNodeId = FindNearestNode(playerPos);


				// lock() して shared_ptr を一時的に取得
				auto pathData = pathData_.lock();
				if (!pathData) return;

				// 自分が今いる位置が、そのノードの近くであるか判定
				const auto* wayList = pathData->GetWayList();
				float distance = VSize(VSub(transform_->pos_, (*wayList)[playerNearNodeId].pos));

				// プレイヤーを見失っているかつ、
				// プレイヤーに一番近いノードまで近づいているなら
				if (distance < PLAYER_NODE_ARRIVE_DIST)
				{
					// 現在位置に一番近いノードから巡回を再開する準備をする
					info_.currentNodeId_ = FindNearestNode(transform_->pos_);

					// 履歴ノードをリセット
					info_.prevNodeId_ = -1;

					// 2つ前の履歴ノードをリセット
					info_.prevPrevNodeId_ = -1;

					info_.nextWayPoint_ = (*wayList)[info_.currentNodeId_].pos;

					// 最後にプレイヤーのいた方向を向いてから待機する
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

	// 実際の移動処理
	Move();

	// 移動しているか
	bool isMoving = (VSize(info_.moveDir_) > MOVE_EPSILON);

	if (isMoving)
	{
		// 一定間隔で足音SEを鳴らす
		seTimer_ -= SceneManager::GetInstance()->GetDeltaTime();

		if (seTimer_ <= 0.0f)
		{
			AudioManager::GetInstance()->PlaySE(
							SoundID::SE_ENEMY_YETI_MOVE, 
							&transform_->pos_, 
							SE_RADIUS_MOVE);

			seTimer_ = SE_INTERVAL_CHASE;
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

	// 攻撃アニメーションが終わったら
	if (animation_->IsEnd())
	{
		// 武器の判定を無効化して、追跡状態に戻る
		useWeapon_->SetAlive(false);
		ChangeState(STATE::CHASE);
		return;
	}
}