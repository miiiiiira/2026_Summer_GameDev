#pragma once

#include "../Component.h"

#include <string>
#include <vector>
#include <DxLib.h>

// 前方宣言
class Transform;

class Cart : public Component
{
public:

	// カートのサイズ横幅
	static constexpr float CART_SIZE_WID_RAD = 75.0f;
	// カートのサイズ縦幅  
	static constexpr float CART_SIZE_HIG_RAD = 50.0f;
	// カートのサイズ奥行  
	static constexpr float CART_SIZE_DEPTH_RAD = 95.0f;
	
	// 線形補間の係数
	static constexpr float COEFFICIENT = 0.15f;

	~Cart(void)override;

	// 初期化
	void Init(void) override;
	// 更新処理
	void Update(void)override;
	// 描画処理
	void Draw3D(void) override;

	// モデルIDを返す
	int GetModelId() const { return modelId_; }

	// Transformを返す
	Transform* GetTransform();

	// Transformを返す
	float GetVelocityY(void);

	// 掴まれた状態にする
	void StartGrabbing(VECTOR localPos);

	// 掴まれた状態を終了する
	void EndGrabbed(void);

	// 相対座標を変更
	void SetLocalPos(VECTOR localPos);

private:
	// 重力加速度
	const float GRAVITY = -0.25f;

	// 最大落下速度
	const float MAX_FALL = -10.0f;

	// モデルID
	int modelId_ = -1;

	// Transform
	Transform* trans_;

	float velocityY_ = 0.0f;

	// 掴まれているか　true / 掴まれている, false / 掴まれていない
	bool isGrabbed_;

	// プレイヤーとのローカル座標
	VECTOR localPos_;

	// プレイヤーの位置をみて移動処理を行う
	void TrackingPlayer(void);

	// 重力処理
	void ApplyGravity(void);

	// デバック用描画
	void DrawDebug(void);
};

