#include "../../../../Scene/SceneManager.h"
#include "../../../../Manager/Audio/AudioManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../PlayerController/PlayerController.h"
#include "../../Collider/3DCollider/CapsuleCollider.h"
#include "../../Collider/StageCollider/StageCollider.h"
#include "../../Transform/Transform.h"
#include "../Weapon/WeaponPunch.h"

#include "Statue.h"

namespace
{
	// 大きさ
	constexpr VECTOR SCALE = { 2.5f, 2.5f, 2.5f };

	// 攻撃時移動速度
	constexpr float ATTACK_SPEED_MOVE = 20.0f;

	// 攻撃時吹っ飛び力
	constexpr float ATTACK_JUMP_POWER = 25.0f;

	// 攻撃時ダメージ力
	constexpr float ATTACK_DAMAGE_POWER = 20.0f;

	// 発見時の驚き演出時間
	constexpr float STEP_TIME_SURPRISE = 4.0f;

	// 攻撃時の待機時間
	constexpr float STEP_TIME_ATTACK = 10.0f;

	// 追跡移動速度
	constexpr float SPEED_CHASE = 5.0f;

	// アタックのY座標のオフセット
	constexpr float ATTACK_POS_OFFSET_Y = 80.0f;

	// 視線位置のオフセット
	constexpr float EYE_OFFSET_Y = 100.0f;

	// 画面内判定の高さ間隔
	constexpr float VIEW_CHECK_STEP_Y = 100.0f;

	// 画面内判定を行う点の数
	constexpr int VIEW_CHECK_POINT_NUM = 5;

	// 帰還先への障害物判定で持ち上げる高さ
	constexpr float RETURN_CHECK_OFFSET_Y = 50.0f;

	// レイの半径
	constexpr float RAY_RADIUS = 30.0f;

	// 攻撃範囲
	constexpr float ATTACK_RANGE = 200.0f;

	// 帰還先に到達したとみなす距離
	constexpr float RETURN_ARRIVE_DISTANCE_SQ = 100.0f * 100.0f;

	// 足音SEの届く半径
	constexpr float SE_RADIUS_STATUE = 3500.0f;

	// 足音SEの再生間隔
	constexpr float SE_INTERVAL_CHASE = 1.0f;

	// モデルのローカル回転
	constexpr float MODEL_FORWARD_OFFSET_DEG = 180.0f;
}

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

void Statue::InitParam(void)
{
	// 攻撃時移動速度
	info_.attackMoveSpeed_ = ATTACK_SPEED_MOVE;

	// 攻撃時吹っ飛び力
	info_.attackJumpPow_ = ATTACK_JUMP_POWER;

	// 攻撃時ダメージ力
	info_.attackDamagePow_ = ATTACK_DAMAGE_POWER;

	// 足音SEのタイマーを初期化
	seTimer_ = 0.0f;
}

void Statue::InitTransform(void)
{
	// モデルの大きさと正面補正を設定
	ApplyModelTransform(SCALE, MODEL_FORWARD_OFFSET_DEG);
}

void Statue::InitPost(void)
{
	// 武器が未生成なら生成して初期化する
	if (useWeapon_ == nullptr)
	{
		useWeapon_ = new WeaponPunch();
		useWeapon_->Init();
	}

	// 初期ステートの設定
	ChangeState(STATE::IDLE);
}

void Statue::Update(void)
{
	// 遅延回転処理
	DelayRotate();

	// モデルに回転を反映
	ApplyModelRotation();

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
				DrawLine3D((*wayList)[i].pos_,
							edge.way_.pos_,
							GetColor(255, 255, 0));
			}
		}

		// 現在ノードと自分の位置をマゼンタの線で結ぶ
		if (info_.currentNodeId_ >= 0 && info_.currentNodeId_ < (int)wayList->size())
		{
			DrawLine3D(transform_->pos_,
						(*wayList)[info_.currentNodeId_].pos_,
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

void Statue::SetHomePos(VECTOR pos)
{
	homePos_ = pos;
}

void Statue::ChangeState(STATE state)
{
	// 同じ状態への遷移は無視
	if (state_ == state) return;

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
	// 驚いている時間を設定
	info_.stateTimer_ = STEP_TIME_SURPRISE;
}

void Statue::ChangeChase(void)
{
	// 追跡時の移動速度を設定
	info_.moveSpeed_ = SPEED_CHASE;
}

void Statue::ChangeAttack(void)
{
	// 攻撃を続ける時間を設定
	info_.stateTimer_ = STEP_TIME_ATTACK;

	// プレイヤーの方を向く
	LookPlayer();

	// 敵の位置
	VECTOR enemyAttackPos = transform_->pos_;

	// 攻撃の発生位置は、足元から一定の高さ分だけ上げた位置にする
	enemyAttackPos.y += ATTACK_POS_OFFSET_Y;

	// パンチ攻撃を発動
	useWeapon_->UseWeapon(enemyAttackPos, info_.moveDir_);
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
	info_.stateTimer_ -= SceneManager::GetInstance()->GetDeltaTime();

	if (info_.stateTimer_ < 0.0f)
	{
		// 驚き終了
		ChangeState(STATE::CHASE);
		return;
	}
}

void Statue::UpdateChase(void)
{
	// プレイヤーの画面内に映っているか
	bool isLooked = IsLookedByPlayer();

	// 視線位置
	VECTOR eyeOffset = { 0.0f, EYE_OFFSET_Y, 0.0f };
	VECTOR enemyHead = VAdd(transform_->pos_, eyeOffset);
	VECTOR playerHead = player_->GetCapsule()->GetStart();
	bool isPlayerVisible = !CheckChaseLineCollision(enemyHead, playerHead, RAY_RADIUS);

	// エリア内なら追跡、エリア外なら帰還
	bool isStop = false;
	if (IsPlayerInArea(minAreaPos_, maxAreaPos_))
	{
		isStop = UpdateChaseInArea(isLooked, isPlayerVisible);
	}
	else
	{
		isStop = UpdateReturn(isLooked, isPlayerVisible);
	}

	// 状態が変わった、または見られて止まっているなら、ここで終わる
	if (isStop) return;

	// 決定した移動方向に従って移動する
	Move();

	// 重力処理
	ApplyGravity();

	// 足音SEの更新
	UpdateFootstep(isLooked);
}

void Statue::UpdateAttack(void)
{
	// 攻撃時間を経過時間分減らす
	info_.stateTimer_ -= SceneManager::GetInstance()->GetDeltaTime();

	// 攻撃処理の更新
	useWeapon_->Update();

	// 攻撃時間が終わったら
	if (info_.stateTimer_ < 0.0f)
	{
		// 武器の判定を無効化して、追跡状態に戻る
		useWeapon_->SetAlive(false);
		ChangeState(STATE::CHASE);
		return;
	}
}

bool Statue::IsLookedByPlayer(void) const
{
	// 足元から一定間隔で高さを上げながら、
	// 画面内に入っているか調べる
	VECTOR checkPos = transform_->pos_;

	for (int i = 0; i < VIEW_CHECK_POINT_NUM; i++)
	{
		// 1点でも画面内なら見られている
		if (!CheckCameraViewClip(checkPos)) return true;

		checkPos.y += VIEW_CHECK_STEP_Y;
	}

	return false;
}

bool Statue::ReturnToHomePos(void)
{
	// 帰還先への障害物判定用の座標
	VECTOR returnCheckPos = transform_->pos_;
	returnCheckPos.y += RETURN_CHECK_OFFSET_Y;

	// 帰還先との間に障害物がなければ、まっすぐ戻る
	if (!CheckChaseLineCollision(returnCheckPos, homePos_, RAY_RADIUS))
	{
		return MoveStraightToHomePos();
	}

	// 障害物があるなら、経路をたどって戻る
	if (info_.path_.empty())
	{
		// 敵の位置から帰還先への経路を探す
		StartPath(transform_->pos_, homePos_);
	}

	if (IsPathFinished())
	{
		// 経路を最後までたどり終えたらクリアする
		ClearPath();
		return false;
	}

	// 次のノードへ向かって移動する
	ChaseNode();
	return false;
}

bool Statue::MoveStraightToHomePos(void)
{
	// 帰還先へのベクトルを計算
	VECTOR diff = VSub(homePos_, transform_->pos_);
	diff.y = 0.0f;

	// ベクトルの正規化で単位ベクトルを取得
	info_.moveDir_ = VNorm(diff);

	// 回転はY軸のみ
	transform_->angle_.x = transform_->angle_.z = 0.0f;

	// 帰還先までの距離を計算
	float distanceToHomePosSQ = GetDistanceSQ(transform_->pos_, homePos_);

	// まだ遠ければ移動を続ける
	if (distanceToHomePosSQ > RETURN_ARRIVE_DISTANCE_SQ) return false;

	// 帰還先に十分近づいたら待機状態に戻る
	ChangeState(STATE::IDLE);
	return true;
}

bool Statue::UpdateChaseInArea(bool isLooked, bool isPlayerVisible)
{
	VECTOR enemyPos = transform_->pos_;
	VECTOR playerPos = player_->GetTransform()->pos_;

	// 攻撃範囲内にいたら、攻撃状態にする
	float distance = VSize(VSub(playerPos, enemyPos));
	if (distance <= ATTACK_RANGE)
	{
		ChangeState(STATE::ATTACK);
		return true;
	}

	// プレイヤーに見られている間は動かない
	if (isLooked)
	{
		ClearPath();
		return true;
	}

	// プレイヤーが直接見えているなら、経路を使わず直進する
	if (isPlayerVisible)
	{
		ClearPath();
		ChaseDirect();
		return false;
	}

	// 障害物があるなら、経路を探してたどる
	if (info_.path_.empty())
	{
		// 敵の位置からプレイヤーの位置への経路を探す
		StartPath(enemyPos, playerPos);
		return false;
	}

	if (IsPathFinished())
	{
		// 経路を最後までたどり終えたらクリアして次回再探索する
		ClearPath();
		return false;
	}

	// 経路がまだ残っているなら次のノードへ向かって移動する
	ChaseNode();
	return false;
}

bool Statue::UpdateReturn(bool isLooked, bool isPlayerVisible)
{
	// 見られている間は経路をクリアしておく
	if (isLooked)
	{
		ClearPath();
	}

	// 見られていて、プレイヤーからも直接見えているなら、その場で止まる
	if (isLooked && isPlayerVisible)
	{
		info_.moveDir_ = Math::VECTOR_ZERO;
		return false;
	}

	// それ以外は帰還先へ戻る
	return ReturnToHomePos();
}

void Statue::UpdateFootstep(bool isLooked)
{
	// 見られている、または止まっている間はタイマーをリセット
	if (isLooked || !IsMoving())
	{
		seTimer_ = 0.0f;
		return;
	}

	// 一定間隔で足音SEを鳴らす
	seTimer_ -= SceneManager::GetInstance()->GetDeltaTime();
	if (seTimer_ > 0.0f) return;

	AudioManager::GetInstance()->PlaySE(
		SoundID::SE_ENEMY_STATUE,
		&transform_->pos_,
		SE_RADIUS_STATUE);

	seTimer_ = SE_INTERVAL_CHASE;
}