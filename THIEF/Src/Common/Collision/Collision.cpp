#include <DxLib.h>
#include <algorithm>

#include "Collision.h"
#include "../Math/Math.h"

namespace Collision
{
    bool HitSpheres(
        const VECTOR& a, const float& ar,
        const VECTOR& b, const float& br)
    {
        float r = ar + br;
        return Math::Distance(a, b) < r;
    }

    bool HitSphereCapsule(
        const VECTOR& sphPos, const float& sphRadius,
        const VECTOR& capA, const VECTOR& capB, const float& capRadius)
    {
        VECTOR ab = VSub(capB, capA);
        VECTOR dir = VNorm(ab);
        float t = VDot(dir, VSub(sphPos, capA));
        t = std::clamp(t, 0.0f, Math::MagnitudeF(ab));

        VECTOR closest = VAdd(capA, VScale(dir, t));
        return HitSpheres(closest, capRadius, sphPos, sphRadius);
    }

    bool HitLineSphere(
        const VECTOR& lineStart, const VECTOR& lineEnd,
        const VECTOR& sphPos, const float& sphRadius)
    {
        VECTOR AB = VSub(lineEnd, lineStart);
        VECTOR AC = VSub(sphPos, lineStart);

        // 線分ABの長さの2乗
        float abLenSq = VSquareSize(AB);

        // 点のプロジェクション（位置比率 t）を計算
        float t = 0.0f;
        if (abLenSq > 0.00001f) // ゼロ除算を防止
        {
            t = VDot(AC, AB) / abLenSq;
        }

        // 最近傍点Pが線分の範囲内(0.0～1.0)に収まるようにクランプする
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;

        // 線分上の最近傍点Pの座標を割り出す
        VECTOR closestPoint = VAdd(lineStart, VScale(AB, t));

        // 最近傍点Pと球の中心の距離の2乗を計算
        VECTOR diff = VSub(sphPos, closestPoint);

        // ベクトルの長さの2乗
        float distanceSq = VSquareSize(diff);

        // 距離の2乗が、半径の2乗以下なら当たっている
        return distanceSq <= (sphRadius * sphRadius);
    }

    bool HitAABBs(
        const VECTOR& pos1 , const VECTOR& size1,
        const VECTOR& pos2, const VECTOR& size2)
    {
        // XYZ軸それぞれで重なり判定
        return (
            abs(pos1.x - pos2.x) <= (size1.x + size2.x)&&
            abs(pos1.y - pos2.y) <= (size1.y + size2.y)&&
            abs(pos1.z - pos2.z) <= (size1.z + size2.z)
            );
    }

    bool HitPoint2Box(
        const Vector2& pointPos,
        const Vector2& boxPos, const float& sizeX, const float& sizeY)
    {
        bool ret = false;
        
        if (pointPos.x > boxPos.x &&
            pointPos.x < boxPos.x + sizeX &&
            pointPos.y > boxPos.y &&
            pointPos.y < boxPos.y + sizeY)
        {
            ret = true;
        }

        return ret;
    }

    bool HitMouse2Box(
        const Vector2& boxPos, const float& sizeX, const float& sizeY)
    {
        // マウスの位置を調べる
        Vector2 mousePos = InputManager::GetInstance()->GetMousePos();
        return  HitPoint2Box(mousePos, boxPos, sizeX, sizeY);
    }
}
