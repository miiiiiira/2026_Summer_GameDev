#pragma once

#include "../Component.h"
#include <DxLib.h>
#include "LightInfo.h"

// 前方宣言
class Transform;

class Wisp :public Component
{
private:
	// 初期座標
	static constexpr VECTOR DEFAULT_POS = { 0.0f,0.0f,0.0f };

	// 大きさ
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };

	// ライトの範囲
	static constexpr float POINTLIGHT_RANGE_MAX = 3000.0f;
	static constexpr float POINTLIGHT_RANGE_MIN = 500.0f;

	// プレイヤーとライトの相対座標
	static constexpr VECTOR REACH_DEFAULT_LIGHT = { -110.0f,-70.0f,220.0f };
	static constexpr VECTOR REACH_MAX_LIGHT = { -110.0f,0.0f,800.0f };

	// オフセット
	static constexpr VECTOR POINTLIGHT_OFFSET = { 0.0f,70.0f,0.0f };

	// 線形補間の係数
	static constexpr float COEFFICIENT = 0.15f;

	// 距離減衰
	static constexpr float ATTEN_0 = 0.0f;
	static constexpr float LIGHT_POW_MAX = 0.0009f;
	static constexpr float LIGHT_POW_MIN = 0.002f;
	static constexpr float ATTEN_2 = 0.0f;

	// デフォルトのライトカラー
	static constexpr VECTOR DEFAULT_LIGHT_COLOR = { 0x41 , 0x69, 0xe1 };

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

	// 指定されたライト状態にする　true / ライトを付ける , false / ライトを消す
	void SetIsRangeMax(bool flg);

	// ライトの状態を見る　true = 最大値にする処理が行われている / false = 最小値にする処理が行われている
	bool GetIsRangeMax(void);

	// 指定されたテクスチャ番号に変更(MAXを設定するとモデルについていた元の色へ戻す)
	void ChangeLightTexture(LIGHT_TYPE lightType);

private:
	// ポイントライトのハンドル
	int pointLightHandle_ = -1;

	VECTOR pointPos_;

	// モデルのハンドル
	int wispModelId_ = -1;

	// テクスチャId
	int textureId_[static_cast<int>(LIGHT_TYPE::COLOR_MAX)];

	// ライトの光量(小さいほど光量が増す)
	float lightPow_;

	// ライトの範囲
	float range_;

	// 範囲設定を最大値にしているか　true = 最大値にする処理が行われる / false = 最小値にする処理が行われる
	bool isRangeMax_;

	// モデルの大きさ
	VECTOR scale_;

	LIGHT_TYPE lightType;

	// 座標更新処理
	void UpdatePos(void);
	// 範囲更新処理
	void UpdateRange(void);

	// デバッグ用の描画処理
	void DebugDraw(void);
};

