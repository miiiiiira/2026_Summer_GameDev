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
	static constexpr float LANTEERN_RANGE = 5000.0f;

	// プレイヤーとランタンの相対座標
	static constexpr VECTOR REACH_DEFAULT_LIGHT = { -80.0f,-50.0f,180.0f };
	static constexpr VECTOR REACH_MAX_LIGHT = {-80.0f,0.0f,600.0f };

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
	void Draw3D(void)override;

	// モデルIDを返す
	int GetLanternModelId() const { return lanternModelId_; }
	int GetWispModelId() const { return wispModelId_; }

	// Transformを返す
	Transform* GetTransform();

	// 指定されたライト状態にする　true / ライトを付ける , false / ライトを消す
	void SetLight(bool lightFlg);

	// ライトの状態を見る　true / ライトがついている , false / ライトがついていない
	bool GetLight(void);

private:
	// ポイントライトのハンドル
	int pointLightHandle_ = -1;

	// モデルのハンドル
	int lanternModelId_ = -1;
	int wispModelId_ = -1;

	int wispTexture = -1;

	// モデルの大きさ
	VECTOR scale_;

	// モデルの向き
	VECTOR angle_;

	// 座標更新処理
	void UpdatePos(void);

	// デバッグ用の描画処理
	void DebugDraw(void);
};

