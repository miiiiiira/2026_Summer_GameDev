#pragma once
#include <DxLib.h>
#include "../Math/Vector2.h"
#include "../../Input/InputManager.h"

namespace Collision
{
#pragma region 3D

    // 球体同士の衝突判定
    bool HitSpheres(const VECTOR& a, float ar, const VECTOR& b, float br);
    
    // 球体とカプセルの衝突判定
    bool HitSphereCapsule(
        const VECTOR& sphPos, float sphRadius,
        const VECTOR& capA, const VECTOR& capB, float capRadius);

#pragma endregion

#pragma region 2D

    // 点と箱の衝突判定
    bool HitPoint2Box(
        const Vector2& pointPos, const Vector2& boxPos, float sizeX, float sizeY);

    // マウスと箱の衝突判定
    bool HitMouse2Box(
        const Vector2& boxPos, float sizeX, float sizeY);

#pragma endregion
}
