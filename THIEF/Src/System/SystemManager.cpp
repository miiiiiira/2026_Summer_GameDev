#include "../Input/InputManager.h"
#include "SystemManager.h"

SystemManager* SystemManager::instance_ = nullptr;

void SystemManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new SystemManager();
	}
}

SystemManager& SystemManager::GetInstance(void)
{
    return *instance_;
}

void SystemManager::Update(void)
{
	if (isDeviceMouse_)
	{
		if (InputManager::GetInstance()->ChangeDevicePad())
		{
			// 特定のキーを押されたら(A,B,X,Y,Lスティックのどれか)パッドモードにする
			isDeviceMouse_ = false;
		}
	}
	else
	{
		if (InputManager::GetInstance()->ChangeDeviceMouse())
		{
			// 特定のキーを押されたら(マウス左クリック，Q,E,W,A,S,Dのどれか)マウスモードにすす
			isDeviceMouse_ = true;
		}
	}
}

void SystemManager::Destroy()
{
	delete instance_;
	instance_ = nullptr;
}

void SystemManager::ResetGame()
{
	score_ = 0; // スコアを0で初期化

	MouseSensitivity_ = MOUSE_SENSITIVITY;
	PadSensitivity_ = PAD_SENSITIVITY;
}

SystemManager::SystemManager(void)
{
	MouseSensitivity_ = MOUSE_SENSITIVITY;
	PadSensitivity_ = PAD_SENSITIVITY;
	score_ = 0;

	if (GetJoypadNum() == 0)
	{
		isDeviceMouse_ = true;
	}
	else
	{
		isDeviceMouse_ = false;
	}
}
