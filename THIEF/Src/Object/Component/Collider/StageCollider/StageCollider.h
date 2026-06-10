#pragma once
#include "../../Component.h"
#include "../../Transform/Transform.h"
#include "../3DCollider/CapsuleCollider.h"

// 前方宣言
class Stage;

// ステージ衝突コンポーネント
class StageCollider : public Component
{
public:
	void Init() override;		// 初期化
	void Update() override;		// 更新

	// ステージ設定
	void SetStage(Stage* stage) { stage_ = stage; }

	// 接地判定
	bool IsGround() const { return isGround_; }

	// 接地フラグを折る
	void IsGroundFold(void) { isGround_ = false; }

private:

	// 地面との当たり判定
	void FieldCollision();

	// 壁との当たり判定
	void WallCollision();

private:
	// 登れる最大傾斜角
	float slopeLimit_ = 45.0f;

	// 接地フラグ
	bool isGround_ = false;

	// Transform
	Transform* transform_ = nullptr;

	// Capsule
	CapsuleCollider* capsule_ = nullptr;

	// ステージ
	Stage* stage_ = nullptr;

	// ステージTransform
	Transform* stageTransform_ = nullptr;
};
