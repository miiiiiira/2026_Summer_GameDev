#pragma once
#include "../Component.h"
#include <DxLib.h>

// 3D位置コンポーネント
class Transform : public Component
{
	void Update() override;
public:
	// 座標
	VECTOR pos_ = { 0.0f,0.0f,0.0f };
	VECTOR prevPos_ = { 0.0f,0.0f,0.0f };
};
