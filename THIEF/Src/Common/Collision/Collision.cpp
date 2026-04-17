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

    bool HitCircleBox(const Vector2& boxPos, float sizeX, float sizeY)
    {
        int mousePosX = 0;
        int mousePosY = 0;
        // マウスの位置を調べる
        GetMousePoint(&mousePosX, &mousePosY);

        bool ret = false;
        
        if(mousePosX > boxPos.x &&
           mousePosX < boxPos.x + sizeX &&
           mousePosY > boxPos.y &&
           mousePosY < boxPos.y + sizeY)
        {
            ret = true;
		}

        return ret;
    }
}
