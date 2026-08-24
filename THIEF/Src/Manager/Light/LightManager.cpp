#include "LightManager.h"
LightManager* LightManager::instance_ = nullptr;

void LightManager::Destroy(void)
{
	DeleteInstance();
}

void LightManager::ResetLight(void)
{
	// デフォルトカラーにする
	nowLightType_ = LIGHT_TYPE::COLOR_0;
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
