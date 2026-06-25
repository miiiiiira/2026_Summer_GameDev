#pragma once

#include "../Component.h"
#include <DxLib.h>

// 前方宣言
class Transform;

class Wisp :public Component
{
private:
	// 初期座標
	static constexpr VECTOR DEFAULT_POS = { 0.0f,0.0f,0.0f };

	// 大きさ
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };

	// ランタンの範囲
	static constexpr float LANTEERN_RANGE = 3000.0f;

	// プレイヤーとランタンの相対座標
	static constexpr VECTOR REACH_DEFAULT_LIGHT = { -110.0f,-70.0f,220.0f };
	static constexpr VECTOR REACH_MAX_LIGHT = { -110.0f,0.0f,800.0f };

	// オフセット
	static constexpr VECTOR POINTLIGHT_OFFSET = { 0.0f,70.0f,0.0f };

	// 線形補間の係数
	static constexpr float COEFFICIENT = 0.15f;

public:

	// コンストラクタ
	Wisp(void);

	// デストラクタ
	~Wisp(void) override;

	// 初期化処理
	void Init(void)override;
	// 更新処理
	void Update(void)override;
	// 描画処理
	void Draw3D(void)override;

	// モデルIDを返す
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

	VECTOR pointPos_;

	// モデルのハンドル
	int wispModelId_ = -1;

	int wispTexture = -1;

	// モデルの大きさ
	VECTOR scale_;

	// 座標更新処理
	void UpdatePos(void);

	// デバッグ用の描画処理
	void DebugDraw(void);
};

