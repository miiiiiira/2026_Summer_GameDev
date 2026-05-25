#include "ScoreManager.h"

ScoreManager* ScoreManager::instance_ = nullptr;

void ScoreManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ScoreManager();
	}
}

ScoreManager& ScoreManager::GetInstance(void)
{
	return *instance_;
}

void ScoreManager::Destroy()
{
	delete instance_;
	instance_ = nullptr;
}

void ScoreManager::ResetGame()
{
	totalPrice_ = deliveryPrice_ = 0;
}

ScoreManager::ScoreManager(void)
{
	totalPrice_ = deliveryPrice_ = 0;
}
