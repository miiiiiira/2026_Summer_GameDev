#include "PlayerActionCounter.h"

PlayerActionCounter* PlayerActionCounter::instance_ = nullptr;

 PlayerActionCounter::PlayerActionCounter(void)
{
}

void PlayerActionCounter::Init(void)
{
	// カウンター初期化
	for (int i = 0; i < Tutorial::STATE::MAX; i++)
	{
		counter_[i] = 0.0f;
	}
}
