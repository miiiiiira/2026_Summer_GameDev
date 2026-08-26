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

	// カートのサイズ
	static constexpr float CART_SIZE_WID_RAD = 75.0f;	// 横幅
	static constexpr float CART_SIZE_HIG_RAD = 50.0f;	// 縦幅
	static constexpr float CART_SIZE_DEPTH_RAD = 95.0f;	// 奥行

public:

	void Init(void) override;	// 初期化
	void Update(void)override;	// 更新
	void Draw3D(void) override;	// 描画

	int GetModelId() const { return modelId_; }	// モデルIDを返す

	Transform* GetTransform();	// Transformを返す
	
	void StartGrabbing(VECTOR localPos);	// 掴まれた状態にする
	void EndGrabbed(void);					// 掴まれた状態を終了する

	void SetLocalPos(VECTOR localPos);	// 相対座標を変更

private:

	void TrackingPlayer(void);	// プレイヤーの位置をみて移動処理を行う

	void ApplyGravity(void);	// 重力処理
	
	void DrawDebug(void);		// デバック用描画

private:
	
	//重力
	const float GRAVITY = -0.25f;	// 重力加速度
	const float MAX_FALL = -10.0f;	// 最大落下速度

	static constexpr float COEFFICIENT = 0.15f;	// 線形補間の係数

private:

	Transform* trans_;	// Transform

private:

	int modelId_ = -1;	// モデルID

	float velocityY_ = 0.0f;	// 実際にかかる重力

	VECTOR localPos_;	// プレイヤーとのローカル座標

	// 掴まれているか　true / 掴まれている, false / 掴まれていない
	bool isGrabbed_;
};

