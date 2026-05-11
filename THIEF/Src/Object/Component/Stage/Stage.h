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

	// モデルIDを返す
	int GetModelId() const { return modelId_; }

	// Transformを返す
	Transform* GetTransform();

	// ワールド座標に変換
	VECTOR ToWorldPos(VECTOR local);

	// ローカル座標に変換
	VECTOR ToLocalPos(VECTOR world);

private:
	// モデルID
	int modelId_ = -1;
};
