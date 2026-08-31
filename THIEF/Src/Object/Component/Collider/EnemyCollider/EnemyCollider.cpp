#include "../../../Actor/Enemy/EnemyBase.h"
#include "../../../Actor/Enemy/Weapon/WeaponBase.h"
#include "../../../Component/PlayerController/PlayerController.h"
#include "../../../Component/Stage/Stage.h"
#include "../../../Component/Crosshair/Crosshair.h"
#include "../../../Component/Collider/3DCollider/CapsuleCollider.h"
#include "../../../../Common/Collision/Collision.h"
#include "../../../Object.h"

#include "EnemyCollider.h"


void EnemyCollider::Init(void)
{

	enemy_ = GetOwner()->GetComponent<EnemyBase>();
}

void EnemyCollider::Update(void)
{
	// 必要な参照が揃っていない場合は処理しない
	if (!enemy_) return;

	if (!player_) return;

	if (!stage_) return;

	// 各判定処理の実行
	CheckEnemyAttack();
	CollisionEnemy2Player();
	CollisionEnemy2PlayerGrab();
}

void EnemyCollider::CheckEnemyAttack(void)
{
	// 武器の情報
	WeaponBase* useWeapon = enemy_->GetWeapon();
	if (useWeapon == nullptr) return;

	// 攻撃中なら
	if (useWeapon->IsAlive())
	{
		// プレイヤーが無敵時間中なら判定スキップ
		if (player_->GetInvincibleTime() > 0) return;

		// プレイヤーのカプセル当たり判定情報取得
		VECTOR startPos = player_->GetOwner()->GetComponent<CapsuleCollider>()->GetStart();
		VECTOR endPos = player_->GetOwner()->GetComponent<CapsuleCollider>()->GetEnd();
		float radius = player_->GetOwner()->GetComponent<CapsuleCollider>()->GetRadius();

		// ステージモデルIDの取得
		int stageModelId = stage_->GetCollModelId() == -1 ? stage_->GetModelId() : stage_->GetCollModelId();

		// 敵の武器とステージの当たり判定
		MV1_COLL_RESULT_POLY_DIM hits = MV1CollCheck_Sphere
		(
			stageModelId,
			-1,
			useWeapon->GetPos(),
			useWeapon->GetCollisionRadius()
		);

		// 壁・床に当たっていたら攻撃判定を消す
		if (hits.HitNum > 0)
		{
			useWeapon->SetAlive(false);
		}
		MV1CollResultPolyDimTerminate(hits);

		// 敵の武器とプレイヤーの当たり判定
		if (Collision::HitSphereCapsule(useWeapon->GetPos(), useWeapon->GetCollisionRadius(),
			startPos, endPos, radius))
		{
			// プレイヤーにダメージを与える
			player_->SetDamage(static_cast<int>(enemy_->GetAttackDamagePow()));

			VECTOR moveDir = VNorm(VSub(startPos, useWeapon->GetPos()));
			moveDir.y = 0.0f;
			player_->SetHitReact(moveDir, enemy_->GetAttackMoveSpeed(), enemy_->GetAttackJumpPow());
			useWeapon->SetAlive(false);
		}
	}
}

void EnemyCollider::CollisionEnemy2Player(void)
{
	VECTOR playerPos = player_->GetTransform()->pos_;
	VECTOR playerTop = player_->GetCapsule()->GetStart();
	float playerRad = player_->GetCapsule()->GetRadius();

	VECTOR enemyPos = enemy_->GetTransform()->pos_;
	VECTOR enemyTop = enemy_->GetCapsule()->GetStart();
	float enemyRad = enemy_->GetCapsule()->GetRadius();

	VECTOR pushVector = Collision::ExtrusionCollisionCapsule(playerPos, playerTop, playerRad, enemyPos, enemyTop, enemyRad);

	// カプセル1（プレイヤー）の押し出し処理
	playerPos = VAdd(playerPos, pushVector);
	player_->GetTransform()->pos_ = playerPos;

	// キノコ（MUSHNUB）に触れた場合のダメージ判定
	if (enemy_->GetTag() == ENEMY_TAG::MUSHNUB &&
		(pushVector.x > 0.1f || pushVector.y > 0.1f || pushVector.z > 0.1f))
	{
		player_->SetDamage(static_cast<int>(enemy_->GetAttackDamagePow()));
	}

	// カプセル2（敵）の押し出し処理
	enemyPos = VSub(enemyPos, pushVector);
	enemy_->GetTransform()->pos_ = enemyPos;
}

void EnemyCollider::CollisionEnemy2PlayerGrab(void)
{
	// プレイヤーが何かを掴んでいる状態、または無敵状態なら処理を行わない
	if (player_->GetGrabbingState() == GRABBING_STATE::IS_GRABBING ||
		player_->GetInvincibleTime() > 0) return;

	// 敵の種類がキノコじゃなかったら処理をしない
	if (enemy_->GetTag() != ENEMY_TAG::MUSHNUB) return;

	// プレイヤーの視線（レイ）の始点と終点
	VECTOR lineStartPos = player_->GetLineStartPos();
	VECTOR lineEndPos = player_->GetLineEndPos();

	// 線分と敵モデルの衝突判定
	MV1_COLL_RESULT_POLY hitResult = MV1CollCheck_Line(enemy_->GetModelId(), -1, lineStartPos, lineEndPos);
	if (!hitResult.HitFlag) return;

	// 視線上にステージの壁などが遮っていないかチェック
	MV1_COLL_RESULT_POLY stageHitResult =
		MV1CollCheck_Line(stage_->GetModelId(), -1, lineStartPos, hitResult.HitPosition);

	// 遮られている場合は判定しない
	if (stageHitResult.HitFlag) return;

	// クロスヘアの更新
	if (crosshair_ != nullptr)
	{
		crosshair_->ChangeCrosshair(CROSSHAIR_TYPE::CROSSHAIR_CAN_GRAB);
	}

	// 掴もうとしていたらダメージを与える
	if (player_->GetGrabbingState() == GRABBING_STATE::TRY_GRABBING)
	{
		player_->SetDamage(static_cast<int>(enemy_->GetAttackDamagePow()));

		if (crosshair_ != nullptr)
		{
			crosshair_->ChangeCrosshair(CROSSHAIR_TYPE::CROSSHAIR_NOT_GRAB);
		}
	}
}