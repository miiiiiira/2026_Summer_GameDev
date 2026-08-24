#include <DxLib.h>
#include <algorithm>

#include "Collision.h"
#include "../Math/Math.h"
#include "../../Manager/Input/InputManager.h"
#include "../MouseCursor/MouseCursor.h"

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

    bool HitSpherePoint(const VECTOR& sphPos, const float& sphRad, const VECTOR& pos)
    {
        // 各軸の差分（距離）を計算
        VECTOR diff = VSub(pos, sphPos);

        // ベクトルの長さの2乗
        float distanceSq = VSquareSize(diff);

        // 入っている
        return distanceSq <= sphRad * sphRad;
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

    VECTOR ExtrusionCollision(VECTOR pos1, float collRad1, VECTOR pos2, float collRad2)
    {
        VECTOR pushPow = Math::VECTOR_ZERO;

        // 球体と球体の衝突判定
        // ２つの座標間の距離をピタゴラスの定理で算出

        VECTOR distance = VSub(pos1, pos2);

        float dis = distance.x * distance.x + distance.y * distance.y + distance.z * distance.z;

        // お互いの半径を合計する
        float radius = collRad1 + collRad2;

        // 合計した半径の２乗よりも、
        // ２つの座標間の距離が小さければ球体は衝突している
        if (radius * radius > dis && dis != 0.0f)
        {
            float length = sqrtf(dis);
            auto overlap = radius - length;

            // 正規化ベクトル（A -> Bの方向）
            VECTOR vec = VNorm(distance);

            // 重なり量の半分
            float push_half = overlap / 2.0f;

            // 押し出し量を計算
            pushPow = VScale(vec, push_half);
            // 上下の押し出しは行わない
            pushPow.y = 0.0f;
        }

        return pushPow;
    }

    VECTOR ExtrusionCollisionCapsule(VECTOR pos1, VECTOR top1, float radius1, VECTOR pos2, VECTOR top2, float radius2)
    {
        VECTOR pushPow = Math::VECTOR_ZERO;

        // 2つのカプセルの中心軸（線分）の間で、最も近くなる2つの座標を格納する変数    
        VECTOR closePos1, closePos2;

        // 各線分の方向ベクトル
        VECTOR d1 = VSub(top1, pos1); // 線分1の方向
        VECTOR d2 = VSub(top2, pos2); // 線分2の方向
        VECTOR r = VSub(pos1, pos2); // 始点同士の差

        // 各種内積の計算（DXライブラリの VDot を使用）
        float a = VDot(d1, d1); // 線分1の長さの2乗
        float e = VDot(d2, d2); // 線分2の長さの2乗
        float f = VDot(d2, r);

        // 線分上の位置を表す比率（0.0 ～ 1.0）を格納する変数
        float s = 0.0f;
        float t = 0.0f;

        // ゼロ除算（長さ0のカプセル）を防ぐチェックをしつつ比率を計算
        if (a <= 0.0001f && e <= 0.0001f)
        {
            // 両方ともただの点（球体）の場合
            s = 0.0f;
            t = 0.0f;
        }
        else if (a <= 0.0001f)
        {
            // 線分1が点の場合
            s = 0.0f;
            t = f / e;
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;
        }
        else
        {
            float c = VDot(d1, r);
            if (e <= 0.0001f)
            {
                // 線分2が点の場合
                t = 0.0f;
                s = -c / a;
                if (s < 0.0f) s = 0.0f;
                if (s > 1.0f) s = 1.0f;
            }
            else
            {
                // 両方とも正常な線分の場合（一般的なケース）
                float b = VDot(d1, d2);
                float denom = a * e - b * b;

                // 2つの線分が並行でない場合
                if (denom != 0.0f)
                {
                    s = (b * f - c * e) / denom;
                    if (s < 0.0f) s = 0.0f;
                    if (s > 1.0f) s = 1.0f;
                }
                else
                {
                    s = 0.0f; // 平行な場合は始点を基準にする
                }

                t = (b * s + f) / e;

                // 線分2の範囲（0.0 ～ 1.0）にクランプし、それに合わせてsも再調整
                if (t < 0.0f)
                {
                    t = 0.0f;
                    s = -c / a;
                    if (s < 0.0f) s = 0.0f;
                    if (s > 1.0f) s = 1.0f;
                }
                else if (t > 1.0f)
                {
                    t = 1.0f;
                    s = (b - c) / a;
                    if (s < 0.0f) s = 0.0f;
                    if (s > 1.0f) s = 1.0f;
                }
            }
        }

        // 割り出した比率（s, t）を元に、実際の最接近点座標（closePos）を確定させる
        closePos1 = VAdd(pos1, VScale(d1, s));
        closePos2 = VAdd(pos2, VScale(d2, t));

        // 最接近点どうしのベクトルと距離（の2乗）を計算    
        VECTOR distance = VSub(closePos1, closePos2);
        float dis = distance.x * distance.x + distance.y * distance.y + distance.z * distance.z;

        // お互いの半径の合計
        float radius = radius1 + radius2;

        // 半径の合計の2乗よりも、最接近点間の距離が小さければ衝突している
        if (radius * radius > dis)
        {
            VECTOR vec;
            float length;

            // 完全に重なっていない場合の通常処理（元のコードと同じ流れ）
            if (dis > 0.0001f)
            {
                length = sqrtf(dis);
                vec = VNorm(distance);
            }
            else
            {
                // 完全に重なっている場合の安全対策（y=0.0fで消えないようにX軸方向にするのが安全）
                length = 0.0f;
                vec = VGet(1.0f, 0.0f, 0.0f);
            }

            auto overlap = radius - length;

            // お互いに退き合う場合は半分（元のコードと同じ）
            float push_half = overlap / 2.0f;

            // 押し出し量を計算（元のコードと同じ）
            pushPow = VScale(vec, push_half);

            // 上下の押し出しを無効化（元のコードと同じ）
            pushPow.y = 0.0f;
        }

        return pushPow;

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

    bool HitBox2Box(const Vector2& boxPos1, const float& sizeX1, const float& sizeY1, const Vector2& boxPos2, const float& sizeX2, const float& sizeY2)
    {
        bool ret = false;

        if (boxPos1.x + sizeX1 > boxPos2.x &&
            boxPos1.x  < boxPos2.x + sizeX2 &&
            boxPos1.y + sizeY1 > boxPos2.y &&
            boxPos1.y < boxPos2.y + sizeY2)
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

    bool HitMouseImg2Box(const Vector2& boxPos, const float& sizeX, const float& sizeY)
    {
        // マウスの位置を調べる
        Vector2 mousePos = InputManager::GetInstance()->GetMousePos();
        // 左上座標にする
        mousePos.x -= (MouseCursor::MOUSE_IMG_SIZE_WID / 2);
        mousePos.y -= (MouseCursor::MOUSE_IMG_SIZE_HIG / 2);

        return  HitBox2Box(mousePos, MouseCursor::MOUSE_IMG_SIZE_WID, MouseCursor::MOUSE_IMG_SIZE_HIG, boxPos, sizeX, sizeY);
    }
}
