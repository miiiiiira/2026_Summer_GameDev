#pragma once
#include "../../Component.h"
#include "../../Transform/Transform.h"
#include "../3DCollider/CapsuleCollider.h"

// 前方宣言
class Stage;

// ステージ衝突コンポーネント
class StageCollider : public Component
{
	// 定数
private:

	// 登れる最大段差
	static constexpr float STEP_HEIGHT = 15.0f;

	// 1フレーム中に複数回衝突する場合に対応するための最大反復回数
	static constexpr int MAX_BOUNCE = 5;

	// めり込み防止用の押し出し量
	static constexpr float SKIN = 0.01f;

	static constexpr float RADIUS_OFFSET = 10.0f;

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

	// 天井に当たっているかどうか
	bool CeilingColl(void);

private:
	// 小さな段差を登れるか判定する
	bool CanStepUp(const VECTOR& pos, const VECTOR& move, float stepHeight);

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
