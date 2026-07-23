#pragma once
#include <map>
#include <DxLib.h>

class EffectResManager
{
public:

	// エフェクトの種類
	enum class TYPE
	{
		ITEM_BREAK_1
	};

	// インスタンスの生成
	static void CreateInstance(void);
	// インスタンスの取得
	static EffectResManager& GetInstance(void);

	// リソースのロード
	void Load(void);
	// リソースの破棄
	void Destroy(void);
	// エフェクシアのリソースハンドルを取得
	int GetResourceId(TYPE type);

	int PlayEffect(float scale, VECTOR dir, VECTOR pos, EffectResManager::TYPE effectType);

private:

	// 静的インスタンス
	static EffectResManager* instance_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	EffectResManager(void);
	// コピーコンストラクタも同様
	EffectResManager(const EffectResManager& instance) = default;
	// デストラクタも同様
	~EffectResManager(void) = default;

	// エフェクシアのリソースハンドル
	std::map<TYPE, int> resourceIds_;
};


