#pragma once

#include "../../Object/Component/Wisp/LightInfo.h"

class LightManager
{
public:
	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new LightManager(); } }
	static LightManager* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

	LightManager();	// コンストラクタ

	void Destroy(void);	// 解放
	
	void ResetLight(void);	// デフォルトカラーにする

	LIGHT_TYPE GetLightType(void);	// ライトのタイプを渡す
	void SetLightType(LIGHT_TYPE lightType);	// ライトのタイプを保持させる

private:

	// 静的インスタンス
	static LightManager* instance_;

	// 現在のライトタイプ
	LIGHT_TYPE nowLightType_;

	// コピー・ムーブ操作を禁止
	LightManager(const LightManager&) = delete;
	LightManager& operator=(const LightManager&) = delete;
	LightManager(LightManager&&) = delete;
	LightManager& operator=(LightManager&&) = delete;
};

