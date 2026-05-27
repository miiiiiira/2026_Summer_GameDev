#pragma once
#include "../Math/Vector2.h"
#include "../Manager/Input/InputManager.h"

namespace Collision
{
#pragma region 3D

    // 球体同士の衝突判定
    bool HitSpheres(
        const VECTOR& a, const float& ar,
        const VECTOR& b, const float& br);
    
    // 球体とカプセルの衝突判定
    bool HitSphereCapsule(
        const VECTOR& sphPos, const float& sphRadius,
        const VECTOR& capA, const VECTOR& capB, const float& capRadius);

    // 線と球体の衝突判定
    bool HitLineSphere(
        const VECTOR& lineStart, const VECTOR& lineEnd,
        const VECTOR& sphPos,const float& sphRadius);

    // AABB同士の衝突判定(中心座標、中心から端までの半径)
    bool HitAABBs(
        const VECTOR& pos1,const VECTOR& size1, 
        const VECTOR& pos2,const VECTOR& size2);

#pragma endregion

#pragma region 2D

    // 点と箱の衝突判定
    bool HitPoint2Box(
        const Vector2& pointPos,
        const Vector2& boxPos, const float& sizeX, const float& sizeY);

    // マウスと箱の衝突判定
    bool HitMouse2Box(
        const Vector2& boxPos, const float& sizeX, const float& sizeY);

#pragma endregion
}
