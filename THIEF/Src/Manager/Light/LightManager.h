#pragma once

#include "../../Object/Component/Wisp/LightInfo.h"

class LightManager
{
public:

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new LightManager(); } }
	static LightManager* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

private:

	static LightManager* instance_;	// 静的インスタンス

	// コピー・ムーブ操作を禁止
	LightManager(const LightManager&) = delete;
	LightManager& operator=(const LightManager&) = delete;
	LightManager(LightManager&&) = delete;
	LightManager& operator=(LightManager&&) = delete;

public:

	LightManager(void);	// コンストラクタ

	void Destroy(void);	// 解放
	
public:

	// デフォルトカラーにする
	void ResetLight(void);	

	// ライトの種類を返す
	LIGHT_TYPE GetLightType(void) { return nowLightType_; }	
	// ライトの種類を保存する
	void SetLightType(LIGHT_TYPE lightType){ nowLightType_ = lightType; }

private:

	LIGHT_TYPE nowLightType_;	// 現在のライトタイプ
};

