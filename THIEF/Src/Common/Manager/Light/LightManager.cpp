#include "LightManager.h"
LightManager* LightManager::instance_ = nullptr;

void LightManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new LightManager();
	}
}

LightManager& LightManager::GetInstance(void)
{
	return *instance_;
}

void LightManager::Destroy(void)
{
	delete instance_;
	instance_ = nullptr;
}

void LightManager::ResetLight(void)
{
	// デフォルト状態にする
	nowLightType_ = LIGHT_TYPE::COLOR_MAX;
}

LIGHT_TYPE LightManager::GetLightType(void)
{
	return nowLightType_;
}

void LightManager::SetLightType(LIGHT_TYPE lightType)
{
	nowLightType_ = lightType;
}

LightManager::LightManager(void)
{
	ResetLight();
}
