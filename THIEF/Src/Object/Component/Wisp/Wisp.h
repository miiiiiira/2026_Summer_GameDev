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

	// Transform
	Transform* trans_ = nullptr;

	// アニメーション
	Animation* anim_ = nullptr;

private:

	// ポイントライト
	int pointLightHandle_ = -1;				// ハンドル
	VECTOR pointPos_ = { 0.0f,0.0f,0.0f };	// 座標
	float lightPow_ = 0.0f;					// ライトの光量(小さいほど光量が増す)
	float range_ = 0.0f;					// ライトの範囲
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

