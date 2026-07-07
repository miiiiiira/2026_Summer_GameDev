#pragma once
#include "../Component.h"

#include <string>
#include <vector>
#include <DxLib.h>

// 前方宣言
class Transform;
class Item;

// ステージコンポーネント
class Stage : public Component
{
public:

	// 納品場所のサイズ横幅
	static constexpr float DELIVERY_SIZE_WID_RAD = 255.0f;
	// 納品場所のサイズ縦幅
	static constexpr float DELIVERY_SIZE_HIG_RAD = 220.0f;
	// 納品場所のサイズ奥行
	static constexpr float DELIVERY_SIZE_DEPTH_RAD = 285.0f;

	// 納品完了スイッチの半径
	static constexpr float DONE_SWITCH_RAD= 20.0f;

	~Stage(void)override;

	// 初期化
	void Init(void) override;
	void Draw3D(void) override;
	
	// モデルIDを返す
	int GetModelId() const { return modelId_; }

	// Transformを返す
	Transform* GetTransform();

	// 納品場所の座標を返す
	VECTOR GetDeliveryPos(void);

	// 納品完了スイッチの座標を返す
	VECTOR GetDoneSwitchPos(void);

	std::vector<Item*> GetItems(void);

	// アイテム設定
	void SetItem(Item* items);

	// ワールド座標に変換
	VECTOR ToWorldPos(VECTOR local);

	// ローカル座標に変換
	VECTOR ToLocalPos(VECTOR world);

private:
	// モデルID
	int modelId_ = -1;

	std::vector<Item*> items_;

	// 納品場所の座標
	VECTOR deliveryPos_;

	// 納品完了スイッチの座標
	VECTOR doneSwitchPos_;

	// デバック用描画
	void DrawDebug(void);
};
