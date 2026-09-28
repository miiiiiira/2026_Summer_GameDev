#pragma once

#include <string>
#include <vector>
#include <DxLib.h>

#include "../Component.h"

class Transform;

class Cart : public Component
{
public:

	// カート初期位置
	static constexpr VECTOR CART_POS_STAGE_1 = { 500.0f,30.0f,0.0f };	// ステージ1
	static constexpr VECTOR CART_POS_STAGE_2 = { 0.0f,30.0f,500.0f };	// ステージ2
	static constexpr VECTOR CART_POS_STAGE_3 = { 500.0f,30.0f,0.0f };	// ステージ3

	// カート初期角度
	static constexpr VECTOR CART_ANGLE_STAGE_1 = { 0.0f,90.0f * (DX_PI_F / 180.0f),0.0f };	// ステージ1
	static constexpr VECTOR CART_ANGLE_STAGE_2 = { 0.0f,0.0f,0.0f };						// ステージ2
	static constexpr VECTOR CART_ANGLE_STAGE_3 = { 0.0f,90.0f * (DX_PI_F / 180.0f),0.0f };	// ステージ3

	// カートのサイズ
	static constexpr float CART_SIZE_WID_RAD = 75.0f;	// 横幅
	static constexpr float CART_SIZE_HIG_RAD = 50.0f;	// 縦幅
	static constexpr float CART_SIZE_DEPTH_RAD = 95.0f;	// 奥行

	// カプセルコライダー用大きさ
	static constexpr float HEIGHT = 70.0f;
	static constexpr float BOTTOM = 30.0f;

	static constexpr float WIDTH = 35.0f;
	static constexpr float DEPTH = 50.0f;
	static constexpr float RADIUS = 60.0f;

public:

	void Init(void) override;	// 初期化
	void Update(void)override;	// 更新
	void Draw3D(void) override;	// 描画

public:

	// モデルIDを返す
	int GetModelId() const { return modelId_; }	

	// Transformを返す
	Transform* GetTransform(void) { return trans_; }

	// 掴まれた状態にする
	void StartGrabbing(VECTOR localPos) { isGrabbed_ = true, localPos_ = localPos; }

	// 掴まれた状態を終了する
	void EndGrabbed(void) { isGrabbed_ = false; }

	// 相対座標を変更
	void SetLocalPos(VECTOR localPos) { localPos_ = localPos; }

private:
	
	//重力
	const float GRAVITY = -0.25f;	// 重力加速度
	const float MAX_FALL = -10.0f;	// 最大落下速度

	// 線形補間の係数
	static constexpr float COEFFICIENT = 0.15f;	

private:

	// Transform
	Transform* trans_ = nullptr;	

private:

	// モデルID
	int modelId_ = -1;

	// 実際にかかる重力
	float velocityY_ = 0.0f;	

	// プレイヤーとのローカル座標
	VECTOR localPos_ = { 0.0f,0.0f,0.0f };

	// 掴まれているか　true / 掴まれている, false / 掴まれていない
	bool isGrabbed_ = false;

private:

	// プレイヤーの位置をみて移動処理を行う
	void TrackingPlayer(void);

	// 重力処理
	void ApplyGravity(void);

	// デバック用描画
	void DrawDebug(void);
};

