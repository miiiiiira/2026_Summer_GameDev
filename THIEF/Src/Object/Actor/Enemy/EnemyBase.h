#pragma once
#include <vector>
#include <string>
#include <memory>
#include <DxLib.h>
#include "../../Component/Component.h"
#include "EnemyCommon.h"


// 前方宣言
class Transform;
class CapsuleCollider;
class StageCollider;
class Animation;
class WeaponBase;
class PlayerController;
class StagePathData;

class EnemyBase : public Component
{
public:
	
	// コンストラクタ
	EnemyBase(void);
	// デストラクタ
	virtual ~EnemyBase(void) override;

	void Init(void) override;
	void Update(void) override = 0;
	void Draw3D(void) override;
	void Draw2D(void) override;

	// 初期データの読み込みとセット
	void SetPathData(PlayerController* player, int stageId, std::shared_ptr<StagePathData> pathData);
	void SetEnemyData(const EnemyData& data);

	// ゲッター・セッター
	bool IsAlive(void) const{ return info_.isAlive_; }

	Transform* GetTransform();	// Transformを返す

	CapsuleCollider* GetCapsule(void);	// CapsuleColliderを返す

	WeaponBase* GetWeapon(void);	// WeaponBaseを返す

	float GetAttackDamagePow(void) const;
	float GetAttackMoveSpeed(void) const;
	float GetAttackJumpPow(void) const;

	ENEMY_TAG GetTag(void) const;

	// モデルIDを返す
	int GetModelId() const { return info_.modelId_; }

	void SetPos(VECTOR pos);

protected:

	// コンポーネント保持用
	Transform* transform_ = nullptr;
	CapsuleCollider* capColl_ = nullptr;
	StageCollider* stageColl_ = nullptr;
	Animation* anim_ = nullptr;

	// 外部参照
	PlayerController* player_ = nullptr;
	WeaponBase* useWeapon_ = nullptr;
	std::weak_ptr<StagePathData> pathData_ = {};

	EnemyInfo info_;
	int stageId_ = -1;

protected:

	void FindPath(int startNodeId, int goalNodeId);

	// 移動方向に応じた遅延回転
	void DelayRotate(void);

	// プレイヤー追従処理
	void LookPlayer(void);

	// 移動処理
	void Move(void);

	float GetDistance(VECTOR pos1, VECTOR pos2);

	// プレイヤーを見つけたかどうか
	bool CheckPlayerDiscovery(float radius);

	// プレイヤーが特定のエリアにいるかどうか
	bool IsPlayerInArea(VECTOR minPos, VECTOR maxPos);

	// ジャンプ処理
	void Jump(void);

	// 重力処理
	void ApplyGravity();

	void SetMoveDirPatrol(void);

	// 次のノードを選ぶ
	int SelectNextNode(void);
	// ノード到着時
	void ArriveNode(void);

	// 一番近いノードを探す
	int FindNearestNode(VECTOR pos);

	// ノードを経由して追従
	void ChaseNode(void);
	// 直接追従
	void ChaseDirect(void);

	// 追従用の線分かステージと当たっているかどうか
	bool CheckChaseLineCollision(VECTOR pPos, VECTOR ePos, float radius);

private:
	static constexpr float JUMP_POW = 25.0f;	// ジャンプ力
	static constexpr float GRAVITY = -1.98f;	// 重力加速度
	static constexpr float MAX_FALL = -40.0f;	// 最大落下速度
};