#pragma once

#include "../../../Object/Component/Wisp/LightInfo.h"

class LightManager
{
public:
	// 目標金額をアイテム全体の金額の60パーセントとする
	static constexpr float TARGET_PRICE_RATIO = 0.5f;

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static LightManager& GetInstance(void);

	// 解放処理
	void Destroy(void);

	//リセット
	void ResetLight(void);

	LIGHT_TYPE GetLightType(void);
	void SetLightType(LIGHT_TYPE lightType);

private:

	// 静的インスタンス
	static LightManager* instance_;

	// 現在のライトタイプ
	LIGHT_TYPE nowLightType_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	LightManager(void);
	// コピーコンストラクタも同様
	LightManager(const LightManager& manager) = default;
	// デストラクタも同様
	~LightManager(void) = default;
};

