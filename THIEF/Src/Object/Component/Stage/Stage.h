#pragma once
#include "../Component.h"

#include <string>
#include <DxLib.h>

// 前方宣言
class Transform;
class Item;

// ステージコンポーネント
class Stage : public Component
{
public:

	// 納品場所のサイズ横幅
	static constexpr float DELIVERY_LOCATION_SIZE_WID = 100.0f;
	// 納品場所のサイズ縦幅
	static constexpr float DELIVERY_LOCATION_SIZE_HIG = 100.0f;

	// 初期化
	void Init(void) override;
	void Draw(void) override;

	// モデルIDを返す
	int GetModelId() const { return modelId_; }

	// Transformを返す
	Transform* GetTransform();

	// 納品場所の座標を返す
	VECTOR GetDeliveryPos(void);

	Item* GetItem(void);

	// アイテム設定
	void SetItem(Item* item);

	// ワールド座標に変換
	VECTOR ToWorldPos(VECTOR local);

	// ローカル座標に変換
	VECTOR ToLocalPos(VECTOR world);

private:
	// モデルID
	int modelId_ = -1;

	Item* item_ = nullptr;

	// 納品場所の座標
	VECTOR deliveryPos_;

	// デバック用描画
	void DebugDraw(void);
};
