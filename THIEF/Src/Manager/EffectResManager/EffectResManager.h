#pragma once
#include <map>
#include <DxLib.h>

class EffectResManager
{
public:

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new EffectResManager(); } }
	static EffectResManager* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

private:

	// 静的インスタンス
	static EffectResManager* instance_;

	EffectResManager(void);		// コンストラクタ
	~EffectResManager(void);	// デストラクタ

	// コピー・ムーブ操作を禁止
	EffectResManager(const EffectResManager&) = delete;
	EffectResManager& operator=(const EffectResManager&) = delete;
	EffectResManager(EffectResManager&&) = delete;
	EffectResManager& operator=(EffectResManager&&) = delete;

public:

	// エフェクトの種類
	enum class TYPE
	{
		ITEM_BREAK_AMPHORA,	// 大きいツボ
		ITEM_BREAK_BOTTLE,	// 茶色瓶
		ITEM_BREAK_GOBLET,	// 杯
		ITEM_BREAK_JAR,		// 食用瓶
		ITEM_BREAK_MUG,		// 木のマグカップ
		ITEM_BREAK_POTION,	// ポーション
		ITEM_BREAK_SKULL,	// 頭蓋骨
	};

public:

	// リソースのロード
	void Load(void);

	// リソースの破棄
	void Destroy(void);

	// エフェクシアのリソースハンドルを取得
	int GetResourceId(TYPE type);

	// エフェクト再生
	int PlayEffect(float scale, VECTOR dir, VECTOR pos, EffectResManager::TYPE effectType);

private:

	// エフェクシアのリソースハンドル
	std::map<TYPE, int> resourceIds_;
};


