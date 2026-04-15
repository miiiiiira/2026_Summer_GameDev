#include "MainMenu.h"

#include <DxLib.h>

#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../../Application.h"

MainMenu::MainMenu(void)
{
}

MainMenu::~MainMenu(void)
{
}

void MainMenu::Init(void)
{
	handle_ = -1;
}

void MainMenu::Load(void)
{
}

void MainMenu::LoadEnd(void)
{
}

void MainMenu::Update(void)
{
}

void MainMenu::Draw(void)
{

	DrawGraph(0, 0, handle_, true);
}

void MainMenu::Release(void)
{
	DeleteGraph(handle_);
}
