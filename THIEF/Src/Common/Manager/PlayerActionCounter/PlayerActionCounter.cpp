#include "PlayerActionCounter.h"

PlayerActionCounter* PlayerActionCounter::instance_ = nullptr;

PlayerActionCounter::PlayerActionCounter()
{
}

PlayerActionCounter::~PlayerActionCounter()
{
}

void PlayerActionCounter::Init(void)
{
	for (int i = 0; i < Tutorial::STATE::MAX; i++)
	{
		counter_[i] = 0.0f;
	}
}
