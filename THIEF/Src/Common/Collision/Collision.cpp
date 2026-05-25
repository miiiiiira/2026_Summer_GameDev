#include <algorithm>

#include "Collision.h"
#include "../Math/Math.h"

namespace Collision
{
    bool HitSpheres(const VECTOR& a, float ar, const VECTOR& b, float br)
    {
        float r = ar + br;
        return Math::Distance(a, b) < r;
    }

    bool HitSphereCapsule(
        const VECTOR& sphPos, float sphRadius,
        const VECTOR& capA, const VECTOR& capB, float capRadius)
    {
        VECTOR ab = VSub(capB, capA);
        VECTOR dir = VNorm(ab);
        float t = VDot(dir, VSub(sphPos, capA));
        t = std::clamp(t, 0.0f, Math::MagnitudeF(ab));

        VECTOR closest = VAdd(capA, VScale(dir, t));
        return HitSpheres(closest, capRadius, sphPos, sphRadius);
    }

    bool HitAABBs(VECTOR pos1 , VECTOR size1, VECTOR pos2, VECTOR size2)
    {
        // XYZ軸それぞれで重なり判定
        return (
            abs(pos1.x - pos2.x) <= (size1.x + size2.x)&&
            abs(pos1.y - pos2.y) <= (size1.y + size2.y)&&
            abs(pos1.z - pos2.z) <= (size1.z + size2.z)
            );
    }

    bool HitPoint2Box(const Vector2& pointPos, const Vector2& boxPos, float sizeX, float sizeY)
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

    bool HitMouse2Box(const Vector2& boxPos, float sizeX, float sizeY)
    {
        // マウスの位置を調べる
        Vector2 mousePos = InputManager::GetInstance()->GetMousePos();
        return  HitPoint2Box(mousePos, boxPos, sizeX, sizeY);
    }
}
