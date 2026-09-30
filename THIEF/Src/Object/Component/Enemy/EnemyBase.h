#pragma once
#include <vector>
#include <string>
#include <memory>
#include <DxLib.h>

#include "../Component.h"
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

	void Init(void) override;			// 初期化
	void Update(void) override = 0;		// 更新
	void Draw3D(void) override;			// 3D描画
	void Draw2D(void) override;			// 2D描画

	// パスデータをセット
	void SetPathData(PlayerController* player, int stageId, 
							std::shared_ptr<StagePathData> pathData);

	// 敵のデータをセット
	void SetEnemyData(const EnemyData& data);

	// 生存しているかどうか
	bool IsAlive(void) const{ return info_.isAlive_; }

	// Transformを返す
	Transform* GetTransform(void) { return transform_; }

	// CapsuleColliderを返す
	CapsuleCollider* GetCapsule(void) { return capColl_; }

	// WeaponBaseを返す
	WeaponBase* GetWeapon(void) { return useWeapon_; }

	// 攻撃力を返す
	float GetAttackDamagePow(void) const { return info_.attackDamagePow_; }

	// 攻撃時の吹っ飛び移動速度を返す
	float GetAttackMoveSpeed(void) const { return info_.attackMoveSpeed_; }

	// 攻撃時の吹っ飛びジャンプ力を返す
	float GetAttackJumpPow(void) const { return info_.attackJumpPow_; }

	// タグを返す
	ENEMY_TAG GetTag(void) const { return info_.tag_; }

	// モデルIDを返す
	int GetModelId() const { return info_.modelId_; }

	// 座標をセットする
	void SetPos(VECTOR pos);

protected:

	// コンポーネント保持用
	Transform* transform_ = nullptr;		// Transformコンポーネント
	CapsuleCollider* capColl_ = nullptr;	// CapsuleColliderコンポーネント
	StageCollider* stageColl_ = nullptr;	// StageColliderコンポーネント
	Animation* animation_ = nullptr;		// Animationコンポーネント

	// 外部参照
	PlayerController* player_ = nullptr;			// プレイヤーのポインタ
	WeaponBase* useWeapon_ = nullptr;				// 武器のポインタ
	std::weak_ptr<StagePathData> pathData_ = {};	// パスデータの弱参照

	EnemyInfo info_;		// 敵の情報
	int stageId_ = -1;		// ステージのモデルID

protected:

	// 経路探索
	void FindPath(int startNodeId, int goalNodeId);

	// 移動方向に応じた遅延回転
	void DelayRotate(void);

	// プレイヤー追従処理
	void LookPlayer(void);

	// 移動処理
	void Move(void);

	// 2点間の距離を返す
	float GetDistanceSQ(VECTOR pos1, VECTOR pos2) { return VSquareSize(VSub(pos1, pos2)); }

	// プレイヤーを見つけたかどうか
	bool CheckPlayerDiscovery(float radius);

	// プレイヤーが特定のエリアにいるかどうか
	bool IsPlayerInArea(VECTOR minPos, VECTOR maxPos);

	// ジャンプ処理
	void Jump(void);

	// 重力処理
	void ApplyGravity();

	// 移動方向を設定する
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

	// ジャンプ力
	static constexpr float JUMP_POW = 25.0f;	

	// 重力加速度
	static constexpr float GRAVITY = -1.98f;	

	// 最大落下速度
	static constexpr float MAX_FALL = -40.0f;	

	// ノードに到達したと判定する距離
	static constexpr float NODE_ARRIVE_DISTANCE = 60.0f;

	// 高低差による発見判定の閾値
	static constexpr float MAX_DETECTION_HEIGHT_DIFFERENCE = 50.0f;

	// 最小移動速度の閾値
	static constexpr float MIN_MOVE_SPEED_THRESHOLD = 0.001f;
};