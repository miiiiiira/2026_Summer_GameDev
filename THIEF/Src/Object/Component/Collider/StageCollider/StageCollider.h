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
	// 初期化
	void Init() override;

	// ステージ設定
	void SetStage(Stage* stage) { stage_ = stage; }

	// 接地判定
	bool IsGround() const { return isGround_; }

	// 接地フラグを折る
	void IsGroundFold(void) { isGround_ = false; }

	// 当たり判定
	void StageColl(float& velocityY);

private:

	// 接地フラグ
	bool isGround_ = false;

	// Transform
	Transform* transform_ = nullptr;

	// Capsule
	CapsuleCollider* capsule_ = nullptr;

	// ステージ
	Stage* stage_ = nullptr;
};
