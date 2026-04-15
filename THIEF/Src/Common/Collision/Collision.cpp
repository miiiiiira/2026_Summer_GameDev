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

    bool HitCircleBox(const VECTOR& sphPos, const VECTOR& boxPos, float sizeX, float sizeY)
    {
        bool ret = false;
        
        if(sphPos.x > boxPos.x &&
           sphPos.x < boxPos.x + sizeX &&
           sphPos.y > boxPos.y &&
           sphPos.y < boxPos.y + sizeY)
        {
            ret = true;
		}

        return ret;
    }
}
