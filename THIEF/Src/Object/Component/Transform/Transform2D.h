#pragma once
#include "../Component.h"
#include <DxLib.h>

#include "../../../Common/Math/Vector2.h"

// 2D位置コンポーネント
class Transform2D : public Component
{
public:
	// 座標
	Vector2 pos_;
};
