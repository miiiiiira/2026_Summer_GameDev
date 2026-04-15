#include "Draw.h"
#include "../Math/Math.h"

namespace Draw
{
    void DrawLineDir(const VECTOR& pos, const VECTOR& dir, int color, float len)
    {
        VECTOR n = Math::Normalize(dir);
        DrawLine3D(
            VAdd(pos, VScale(n, -len)),
            VAdd(pos, VScale(n, len)),
            color
        );
        DrawSphere3D(VAdd(pos, VScale(n, len)), 5, 5, color, color, true);
    }

    void DrawAxis(const VECTOR& pos, const Quaternion& rot, float len)
    {
        DrawLineDir(pos, rot.GetRight(), 0xff0000, len);
        DrawLineDir(pos, rot.GetUp(), 0x00ff00, len);
        DrawLineDir(pos, rot.GetForward(), 0x0000ff, len);
    }
}