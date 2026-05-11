#pragma once
#include "../Component.h"
#include <DxLib.h>

// 3D位置コンポーネント
class Transform : public Component
{
public:
	// 座標
	VECTOR pos_;
};
