#pragma once
#include <DxLib.h>

namespace EnemyCommon 
{
    // 敵もマネージャも使うデータ型をここに隔離する
    struct WAYPOINT 
    {
        int id;
        VECTOR pos;
    };

    struct EDGE
    {
        WAYPOINT way;
        float cost;
    };

    enum class ENEMY_TAG
    {
        YETI,
        MUSHNUB,
        SKELETON,
        STATUE,
        GIGGLE,
        GUNMAN,
        MAX,
    };
}