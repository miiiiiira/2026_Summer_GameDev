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
	static constexpr float CART_SIZE_WID_RAD = 50.0f;
	// カートのサイズ縦幅  
	static constexpr float CART_SIZE_HIG_RAD = 110.0f;
	// カートのサイズ奥行  
	static constexpr float CART_SIZE_DEPTH_RAD = 50.0f;
	
	// プレイヤーとの相対座標
	static constexpr VECTOR CART_LOCAL_POS = {0.0f,5.0f,300.0f};

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

	// 掴まれた状態にする
	void StartGrabbing(void);

	// 掴まれた状態を終了する
	void EndGrabbed(void);

private:
	// モデルID
	int modelId_ = -1;

	// Transform
	Transform* trans_;

	float velocityY_ = 0.0f;

	// カートの向き(Y軸回転のみ使用)
	float angleY_;

	// 掴まれているか　true / 掴まれている, false / 掴まれていない
	bool isGrabbed_;

	// プレイヤーの位置をみて移動処理を行う
	void TrackingPlayer(void);

	// デバック用描画
	void DrawDebug(void);
};

