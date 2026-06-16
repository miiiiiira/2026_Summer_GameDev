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

	// ステージと納品場所までの相対座標
	static constexpr VECTOR DELIVERY_LOCAL_POS = { -922.0f,DELIVERY_SIZE_HIG_RAD,1924.0f };

	// 納品完了スイッチの半径
	static constexpr float DONE_SWITCH_RAD= 20.0f;

	// ステージと納品完了スイッチまでの相対座標
	static constexpr VECTOR DONE_SWITCH_LOCAL_POS = { -1116.0f,150.0f,2255.0f };

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
