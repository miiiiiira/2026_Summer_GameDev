#pragma once

#include <map>
#include <DxLib.h>
#include "../Component.h"
#include "LightInfo.h"

// 前方宣言
class Transform;
class Animation;

class Wisp :public Component
{
public:

	// wispモデルのアニメーションの種類
	enum class ANIM
	{
		NORMAL,		// 通常
		SMALL,		// 小さい
	};

public:

	Wisp(void);					// コンストラクタ
	~Wisp(void) override;		// デストラクタ

	void Init(void)override;	// 初期化
	void Update(void)override;	// 更新

public:

	// モデルIDを返す
	int GetWispModelId() const { return wispModelId_; }

	// 指定されたライト状態にする　true / ライトを付ける , false / ライトを消す
	void SetIsRangeMax(bool flg) { isRangeMax_ = flg; }

	// ライトの状態を見る　true / 最大値にする処理が行われている , false / 最小値にする処理が行われている
	bool GetIsRangeMax(void) { return isRangeMax_; }

	// 指定されたテクスチャ番号に変更(0を設定するとモデルについていた元の色へ戻す)
	void ChangeLightTexture(LIGHT_TYPE lightType);

	// 指定されたアニメーションを再生する
	void SetAnimation(ANIM anim);

private:

	// ライトの範囲
	static constexpr float POINTLIGHT_RANGE_MAX = 1850.0f;	// 最大範囲
	static constexpr float POINTLIGHT_RANGE_MIN = 500.0f;	// 最小範囲

	// プレイヤーとライトの相対座標
	static constexpr VECTOR REACH_DEFAULT_LIGHT = { -110.0f,-70.0f,220.0f };
	static constexpr VECTOR REACH_MAX_LIGHT = { -110.0f,0.0f,800.0f };

	// オフセット
	static constexpr VECTOR POINTLIGHT_OFFSET = { 0.0f,70.0f,0.0f };

	// 線形補間の係数
	static constexpr float COEFFICIENT = 0.15f;

	// 距離減衰
	static constexpr float ATTEN_0 = 0.0f;
	static constexpr float LIGHT_POW_MAX = 0.001f;
	static constexpr float LIGHT_POW_MIN = 0.002f;
	static constexpr float ATTEN_2 = 0.0f;

	// デフォルトのライトカラー
	static constexpr VECTOR DEFAULT_LIGHT_COLOR = { 0xe0,0xe0,0xe0 };

private:

	// Transform
	Transform* trans_ = nullptr;

	// アニメーション
	Animation* anim_ = nullptr;

private:

	// ポイントライト
	int pointLightHandle_ = -1;				// ハンドル
	VECTOR pointPos_ = { 0.0f,0.0f,0.0f };	// 座標
	float lightPow_ = LIGHT_POW_MAX;;		// ライトの光量(小さいほど光量が増す)
	float range_ = POINTLIGHT_RANGE_MAX;	// ライトの範囲
	bool isRangeMax_ = true;				// 範囲設定を最大値にしているか　true / 最大値にする処理が行われる , false / 最小値にする処理が行われる

	// wisp
	int wispModelId_ = -1;							// モデルのハンドル
	std::map<LIGHT_TYPE, int> textures_;			// テクスチャId
	VECTOR scale_ = { 1.0f,1.0f,1.0f };				// モデルの大きさ
	LIGHT_TYPE lightType_ = LIGHT_TYPE::COLOR_0;	// 使用中のライトの種類

	// ライトを奥にしているか　true / 奥 , false / 手前
	bool isPushLight_ = false;

private:

	// 座標更新処理
	void UpdatePos(void);

	// 範囲更新処理
	void UpdateRange(void);

	// プレイヤー側を向く
	void LookPlayer(void);

	// ライトの色を変更できる(デバック時のみ)
	void DebugLightColorChange(void);
};

