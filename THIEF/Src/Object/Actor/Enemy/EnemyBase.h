#pragma once
#include<vector>
#include <DxLib.h>

class AnimationController;
class WeaponBase;
class WeaponPunch;
class PlayerController;

class EnemyBase
{
public:
	
	// ウェイポイント
	struct Waypoint
	{
		int id;			// ウェイポイントID
		VECTOR pos;		// 座標
	};

	// エッジ
	struct Edge
	{
		Waypoint way;	// 行った先のウェイポイントID
		float cost;		// 距離
	};

	// コンストラクタ
	EnemyBase(void);
	// デストラクタ
	virtual ~EnemyBase(void);

	// 読み込み処理
	virtual void Load(void);
	// 初期化処理
	virtual void Init(PlayerController* player,int id = -1) = 0;
	// 更新処理
	virtual void Update(void) = 0;
	// 描画処理
	virtual void Draw(void);
	// 解放処理
	void Release(void);

	// 始点から終点までの最短経路を計算し、エッジのリストとして返す
	void FindPath(int startNodeId, int goalNodeId);

	// 武器クラスを取得
	WeaponBase* GetUseWeapon(void);

	VECTOR GetPos(void);
	void SetPos(VECTOR pos);
	float GetRadius(void);
	VECTOR GetPrevPos(void);
	VECTOR GetStart(void);
	VECTOR GetEnd(void);
	void SetGround(bool isGround);
	float GetVelocity(void);
	void SetVelocity(float velocityY);

protected:

	const float NODE_CONNECT_MAX_DISTANCE_SQ = 1300.0f * 1300.0f;

	// ジャンプ力
	const float JUMP_POW = 25.0f;

	// 重力加速度
	const float GRAVITY = -1.98f;

	// 最大落下速度
	const float MAX_FALL = -40.0f;

	AnimationController* animationController_;

	// 武器
	WeaponBase* useWeapon_;
	WeaponPunch* weaponPunch_;

	PlayerController* player_;

	// 敵の情報
	int modelId_;
	VECTOR scale_;
	VECTOR angle_;
	VECTOR localAngle_;
	VECTOR pos_;
	float radius_;
	VECTOR moveDir_;
	float moveSpeed_;
	VECTOR movePow_;
	VECTOR prevPos_;
	VECTOR jumpPow_;
	float stepJump_;
	bool isJump_;
	bool isNotice_;
	bool isGround_;	
	float velocityY_ = 0.0f;	// 現在の落下速度

	// 開始位置
	VECTOR startOffset_;

	// 終了位置
	VECTOR endOffset_;

	int stageId_ = -1;

	std::vector<Waypoint> way_;					// ウェイポイントを格納
	std::vector<std::vector<Edge>> edgeList_;	// 行動可能な辺を格納
	std::vector<float> minCosts_;				// ポイントへの最短経路合計
	std::vector<int> parentNodes_;				// どこから来たかを記録
	std::vector<Edge> path_;					// 探索された最短経路を格納

	void AddEdge(int fromId, int toId);

	// 移動方向に応じた遅延回転
	void DelayRotate(void);

	// プレイヤー追従処理
	void LookPlayer(void);

	float GetDistance(VECTOR pos1, VECTOR pos2);

	// プレイヤーを見つけたかどうか
	bool CheckPlayerDiscovery(float radius);

	// ジャンプ処理
	void Jump(void);

	// 重力処理
	void ApplyGravity();

private:
	void LoadCsvData(void);
};