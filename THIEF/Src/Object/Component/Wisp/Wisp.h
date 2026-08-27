#pragma once

#include "../Component.h"
#include <DxLib.h>
#include <map>
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
	void Draw3D(void)override;	// 描画

	int GetWispModelId() const { return wispModelId_; }	// モデルIDを返す

	void SetIsRangeMax(bool flg);	// 指定されたライト状態にする　true / ライトを付ける , false / ライトを消す
	
	bool GetIsRangeMax(void);	// ライトの状態を見る　true = 最大値にする処理が行われている / false = 最小値にする処理が行われている

	void ChangeLightTexture(LIGHT_TYPE lightType);	// 指定されたテクスチャ番号に変更(0を設定するとモデルについていた元の色へ戻す)

	void SetAnimation(ANIM anim);	// 指定されたアニメーションを再生する

private:

	void UpdatePos(void);	// 座標更新処理
	
	void UpdateRange(void);	// 範囲更新処理

	void LookPlayer(void);	// プレイヤー側を向く

	void DebugDraw(void);	// デバッグ用の描画処理

private:

	// 初期座標
	static constexpr VECTOR DEFAULT_POS = { 0.0f,0.0f,0.0f };

	// 大きさ
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };

	// ライトの範囲
	static constexpr float POINTLIGHT_RANGE_MAX = 1850.0f;
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
	static constexpr float LIGHT_POW_MAX = 0.001f;
	static constexpr float LIGHT_POW_MIN = 0.002f;
	static constexpr float ATTEN_2 = 0.0f;

	// デフォルトのライトカラー
	static constexpr VECTOR DEFAULT_LIGHT_COLOR = { 0xe0,0xe0,0xe0 };

private:
	
	Transform* trans_;	// Transform

	Animation* anim_;	// アニメーション

private:

	// ポイントライト
	int pointLightHandle_ = -1;	// ハンドル
	VECTOR pointPos_;			// 座標
	float lightPow_;			// ライトの光量(小さいほど光量が増す)
	float range_;				// ライトの範囲
	bool isRangeMax_;			// 範囲設定を最大値にしているか　true / 最大値にする処理が行われる , false / 最小値にする処理が行われる
	
	// wisp
	int wispModelId_ = -1;					// モデルのハンドル
	std::map<LIGHT_TYPE, int> textures_;	// テクスチャId
	VECTOR scale_;							// モデルの大きさ
	LIGHT_TYPE lightType_;					// 使用中のライトの種類
	
	bool isPushLight_;	// ライトを奥にしているか　true / 奥 , false / 手前
};

