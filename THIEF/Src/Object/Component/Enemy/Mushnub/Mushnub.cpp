#include "../../../../Scene/SceneManager.h"
#include "../../../../Manager/Audio/AudioManager.h"
#include "../../../../Common/Math/Math.h"
#include "../../../../Common/Transform/MatrixUtility.h"
#include "../../PlayerController/PlayerController.h"
#include "../../Collider/3DCollider/CapsuleCollider.h"
#include "../../Collider/StageCollider/StageCollider.h"
#include "../../Animation/Animation.h"
#include "../../Transform/Transform.h"
#include "../EnemyCommon.h"

#include "Mushnub.h"

Mushnub::Mushnub(void)
{
}

Mushnub::~Mushnub(void)
{
}

void Mushnub::Init(void)
{
	EnemyBase::Init();

	// アニメーションの初期化
	animation_->Init();

	// アニメーションの追加
	for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
	{
		animation_->AddInFbx(
					static_cast<int>(i), 
					ANIM_SPEED,
					static_cast<int>(i));
	}

	// 敵のデータを取得して設定
	const auto& data = EnemyTable::Table.at(ENEMY_TAG::MUSHNUB);
	SetEnemyData(data);

	// パラメータ初期化
	info_.moveDir_ = Math::VECTOR_ZERO;

	// 巡回半径と視認半径の初期値
	info_.viewRadius_ = VIEW_RADIUS_DEFAULT;
	info_.attackDamagePow_ = ATTACK_DAMAGE_POWER;

	// 敵のタグを設定
	info_.tag_ = ENEMY_TAG::MUSHNUB;

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

	// 初期ステート設定
	ChangeState(STATE::IDLE);
}

void Mushnub::Update(void)
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
	case Mushnub::STATE::IDLE:      
		UpdateIdle();     
		break;

	case Mushnub::STATE::SURPRISE:   
		UpdateSurprise(); 
		break;

	case Mushnub::STATE::CHASE:     
		UpdateChase();    
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

void Mushnub::Draw3D(void)
{
	EnemyBase::Draw3D();

#ifdef _DEBUG

	if (transform_)
	{
		// 敵の当たり判定カプセルを赤いワイヤーフレームで描画
		VECTOR start = VAdd(transform_->pos_, info_.startOffset_);
		VECTOR end = VAdd(transform_->pos_, info_.endOffset_);
		DrawCapsule3D(start, end, info_.radius_, 8, 0xff0000, 0xff0000, false);
	}

	// プレイヤーを検知する行動エリアをマゼンタのワイヤーフレームで描画
	DrawCube3D(minAreaPos_, maxAreaPos_, 0xff00ff, 0xff00ff, false);
#endif
}

void Mushnub::Draw2D(void)
{
}

void Mushnub::SetAreaPos(VECTOR minPos, VECTOR maxPos)
{
	minAreaPos_ = minPos; 
	maxAreaPos_ = maxPos;
}

void Mushnub::SetChasePos(VECTOR pos)
{ 
	chasePos_ = pos; 
}

void Mushnub::ChangeState(STATE state)
{
	state_ = state;

	// 遷移時に一度だけ行う初期化処理を呼び出す
	switch (state_)
	{
	case Mushnub::STATE::IDLE: 
		ChangeIdle(); 
		break;

	case Mushnub::STATE::SURPRISE:
		ChangeSurprise(); 
		break;

	case Mushnub::STATE::CHASE: 
		ChangeChase(); 
		break;

	default:
		break;
	}
}

void Mushnub::ChangeIdle(void)
{
	// 待機する時間を設定
	info_.step_ = STEP_TIME_IDLE;

	// 待機アニメーションをループ再生
	animation_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void Mushnub::ChangeSurprise(void)
{
	// 驚いている時間（猶予時間）を設定
	info_.step_ = STEP_TIME_SURPRISE;

	// プレイヤーの方を向く
	LookPlayer();

	// 発見時の鳴き声SEを再生
	AudioManager::GetInstance()->PlaySE(
					SoundID::SE_ENEMY_MUSHNUB, 
					&transform_->pos_);

	// 驚きのアニメーションを1回だけ再生
	animation_->Play(static_cast<int>(ANIM_TYPE::HIT_REACT), false);
}

void Mushnub::ChangeChase(void)
{
	// 追跡時の移動速度
	info_.moveSpeed_ = SPEED_CHASE;

	// 歩きアニメーションをループ再生
	animation_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
}

void Mushnub::UpdateIdle(void)
{
	// プレイヤーが行動エリア内に入ったら発見状態へ
	if (IsPlayerInArea(minAreaPos_, maxAreaPos_))
	{
		ChangeState(STATE::SURPRISE);
		return;
	}
}

void Mushnub::UpdateSurprise(void)
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

void Mushnub::UpdateChase(void)
{
	if (IsPlayerInArea(minAreaPos_, maxAreaPos_))
	{
		// プレイヤーがエリア内にいる間は、プレイヤーの方を向き続ける
		LookPlayer();
	}
	else
	{
		// プレイヤーがエリア外に出たら、帰還先へ向かう
		// 相手へのベクトルを計算
		VECTOR diff = VSub(chasePos_, transform_->pos_);

		// 高さは無視して水平方向のみで移動させる
		diff.y = 0.0f;

		// ベクトルの正規化で単位ベクトルを取得
		info_.moveDir_ = VNorm(diff);

		// 回転はY軸のみ
		transform_->angle_.x = transform_->angle_.z = 0.0f;

		// 帰還先までの距離を計算
		float distToChasePos = GetDistanceSQ(transform_->pos_, chasePos_);

		// 帰還先に十分近づいたら待機状態に戻る
		if (distToChasePos <= RETURN_THRESHOLD_DISTANCE_SQ)
		{
			ChangeState(STATE::IDLE);
			return;
		}
	}

	// プレイヤーとの距離を計算
	float distToPlayer = GetDistanceSQ(transform_->pos_, player_->GetTransform()->pos_);

	// プレイヤーとの距離が保つべき距離以上なら、歩いて近づく
	if (distToPlayer >= PLAYER_KEEP_DISTANCE_SQ)
	{
		animation_->Play(static_cast<int>(ANIM_TYPE::WALK), true);

		Move();
	}
	else
	{
		// 十分近い場合は、その場で立ち止まって待機アニメーションにする
		animation_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
	}
}