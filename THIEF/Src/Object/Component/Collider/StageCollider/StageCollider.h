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

	// 1フレーム中に複数回衝突する場合に対応するための最大反復回数
	static constexpr int MAX_BOUNCE = 5;

	static constexpr float RADIUS_OFFSET = 10.0f;

public:
	// 初期化
	void Init() override;

	// ステージ設定
	void SetStage(Stage* stage) { stage_ = stage; }

	// 床と判定する法線の設定
	void SetFloorNormalY(float y)
	{
		floorNormalY_ = y;
	}

	// 壁と判定する法線の設定
	void SetWallNormalY(float y)
	{
		wallNormalY_ = y;
	}

	// 急斜面と判定する法線の設定
	void SetSlopeNormalY(float y)
	{
		slopeNormalY_ = y;
	}

	// 段差の設定
	void SetStepHeight(float h) { stepHeight_ = h; }

	// めり込み防止用の押し出し量の設定
	void SetSkin(float skin) { skin_ = skin; }

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

	// 床と判定する法線
	float floorNormalY_ = 0.65f;

	// 壁と判定する法線
	float wallNormalY_ = 0.50f;

	// 急斜面と判定する法線
	float slopeNormalY_ = 0.80f;

	// 段差の設定
	float stepHeight_ = 15.0f;

	// めり込み防止用の押し出し量
	float skin_ = 0.01f;

	// Transform
	Transform* transform_ = nullptr;

	// Capsule
	CapsuleCollider* capsule_ = nullptr;

	// ステージ
	Stage* stage_ = nullptr;
};
