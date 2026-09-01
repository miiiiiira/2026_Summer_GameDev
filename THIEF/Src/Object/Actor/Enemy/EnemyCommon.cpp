#include "EnemyCommon.h"

namespace EnemyTable
{
	const std::unordered_map<ENEMY_TAG, EnemyData> Table =
	{
{
			ENEMY_TAG::SKELETON,
			{
				"Data/Model/Enemy/Skeleton.mv1",
				VGet(0.0f, 80.0f, 0.0f),
				VGet(0.0f, 40.0f, 0.0f),
				50.0f
			}
		},
		{
			ENEMY_TAG::YETI,
			{
				"Data/Model/Enemy/Yeti.mv1",
				VGet(0.0f, 180.0f, 0.0f),
				VGet(0.0f, 40.0f, 0.0f),
				50.0f
			}
		},
		{
			ENEMY_TAG::MUSHNUB,
			{
				"Data/Model/Enemy/Mushnub_Evolved.mv1",
				VGet(0.0f, 80.0f, 0.0f),
				VGet(0.0f, 40.0f, 0.0f),
				50.0f
			}
		},
		{
			ENEMY_TAG::STATUE,
			{
				"Data/Model/Enemy/Statue.mv1",
				VGet(0.0f, 400.0f, 0.0f),
				VGet(0.0f, 80.0f, 0.0f),
				80.0f
			}
		},
		{
			ENEMY_TAG::GIGGLE,
			{
				// ìñÇΩÇËîªíËÅAÉÇÉfÉãÇ»Çµ
				"",
				VGet(0.0f, 0.0f, 0.0f),
				VGet(0.0f, 0.0f, 0.0f),
				0.0f
			}
		},
	};
}