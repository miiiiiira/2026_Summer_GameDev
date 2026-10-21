#include "LightManager.h"
LightManager* LightManager::instance_ = nullptr;

void LightManager::ResetLight(void)
{
	// デフォルトカラーにする
	nowLightType_ = LIGHT_TYPE::COLOR_0;
}

LightManager::LightManager(void)
{
	ResetLight();
}
