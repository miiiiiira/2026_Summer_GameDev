#pragma once
#include "../Component.h"

#include <string>
#include <DxLib.h>

// 前方宣言
class Transform;

// ステージコンポーネント
class Stage : public Component
{
public:
	// 初期化
	void Init(void) override;
	void Draw(void) override;

	// モデルIDを返す
	int GetModelId() const { return modelId_; }

	// Transformを返す
	Transform* GetTransform();

	// 納品場所の座標を返す
	VECTOR GetDeliveryPos(void);

	// ワールド座標に変換
	VECTOR ToWorldPos(VECTOR local);

	// ローカル座標に変換
	VECTOR ToLocalPos(VECTOR world);

private:
	// モデルID
	int modelId_ = -1;

	// 納品場所の座標
	VECTOR deliveryPos_;

	// デバック用描画
	void DebugDraw(void);
};
