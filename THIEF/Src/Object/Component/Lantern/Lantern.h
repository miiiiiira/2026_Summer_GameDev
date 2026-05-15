#pragma once

#include "../Component.h"
#include <DxLib.h>

// 前方宣言
class Transform;

class Lantern:public Component
{
private:
	// 大きさ
	static constexpr VECTOR SCALE = { 0.0f,0.0f,0.0f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f,0.0f,0.0f };

	// ランタンの範囲
	static constexpr float LANTEERN_RANGE = 400.0f;

	// プレイヤーとランタンの相対座標
	static constexpr VECTOR REACH_DEFAULT_LIGHT = { -80.0f,-50.0f,150.0f };
	static constexpr VECTOR REACH_MAX_LIGHT = {-80.0f,0.0f,350.0f };

	// 線形補間の係数
	static constexpr float COEFFICIENT = 0.1f;

public:

	// コンストラクタ
	Lantern(void);

	// デストラクタ
	~Lantern(void) override;

	// 初期化処理
	void Init(void)override;
	// 更新処理
	void Update(void)override;
	// 描画処理
	void Draw(void)override;

	// モデルIDを返す
	int GetModelId() const { return modelId_; }

	// Transformを返す
	Transform* GetTransform();

	// ワールド座標に変換
	VECTOR ToWorldPos(VECTOR local);

	// ローカル座標に変換
	VECTOR ToLocalPos(VECTOR world);

	// カメラの座標と向きを参照できるようにする
	void SetCameraPosAngle(VECTOR* cameraPos, VECTOR* cameraAngle);
private:
	// ポイントライトのハンドル
	int pointLightHandle_ = -1;

	// モデルのハンドル
	int modelId_ = -1;

	// モデルの大きさ
	VECTOR scale_;

	// モデルの向き
	VECTOR angle_;

	// カメラの座標
	VECTOR* cameraPos_;

	// カメラの向き
	VECTOR* cameraAngle_;

	// 座標更新処理
	void UpdatePos(void);

	// デバッグ用の描画処理
	void DebugDraw(void);
};

