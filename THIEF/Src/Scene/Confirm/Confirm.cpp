#include "Confirm.h"

#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../../Common/Collision/Collision.h"
#include "../MainMenu/MainMenu.h"

Confirm::Confirm(void)
{
	confirmImg_ = -1;
	quitImg_ = -1;
	mainMenuImg_ = -1;
	yesImg_ = -1;
	noImg_ = -1;
	frameImg_ = -1;
}

Confirm::~Confirm(void)
{
}

void Confirm::Init(void)
{
	ChangeSelect(SELECT::NONE);

	switch (result_)
	{
	case RESULT::QUIT:
		confirmImg_ = quitImg_;
		break;
	case RESULT::MAIN_MENU:
		confirmImg_ = mainMenuImg_;
		break;
	}

}

void Confirm::Load(void)
{
	quitImg_ = LoadGraph((Application::PATH_IMAGE + "ConfirmQuit.png").c_str());
	mainMenuImg_ = LoadGraph((Application::PATH_IMAGE + "ConfirmMainMenu.png").c_str());
	yesImg_ = LoadGraph((Application::PATH_IMAGE + "Yes.png").c_str());
	noImg_ = LoadGraph((Application::PATH_IMAGE + "No.png").c_str());
	frameImg_ = LoadGraph((Application::PATH_IMAGE + "frame.png").c_str());
}

void Confirm::LoadEnd(void)
{
	Init();
}

void Confirm::Update(void)
{
	// 衝突判定
	if (Collision::HitCircleBox({ YES_POS_X, YES_POS_Y }, IMAGE_SIZE_X, IMAGE_SIZE_Y))
	{
		ChangeSelect(SELECT::YES);
	}
	else if (Collision::HitCircleBox({ NO_POS_X, NO_POS_Y }, IMAGE_SIZE_X, IMAGE_SIZE_Y))
	{
		ChangeSelect(SELECT::NO);
	}
	else
	{
		ChangeSelect(SELECT::NONE);
	}

	// マウスを左クリックしたら
	if (InputManager::GetInstance()->IsClickMouseLeft())
	{
		switch (currentSelect_)
		{
		case Confirm::SELECT::YES:
			UpdateYes();
			break;
		case Confirm::SELECT::NO:
			UpdateNo();
			break;
		default:
			break;
		}
	}
}

void Confirm::Draw(void)
{
	DrawGraph(CONFIRM_POS_X, CONFIRM_POS_Y, confirmImg_, true);				// 確認の文字を表示
	DrawExtendGraph(currentSelectPos_.x, currentSelectPos_.y,
		currentSelectPos_.x + IMAGE_SIZE_X,
		currentSelectPos_.y + IMAGE_SIZE_Y,
		frameImg_, true);
	DrawGraph(YES_POS_X, YES_POS_Y, yesImg_, true);							// YESの文字を表示
	DrawGraph(NO_POS_X, NO_POS_Y, noImg_, true);							// NOの文字を表示
}

void Confirm::Release(void)
{
	confirmImg_ = -1;

	DeleteGraph(quitImg_);
	DeleteGraph(mainMenuImg_);
	DeleteGraph(yesImg_);
	DeleteGraph(noImg_);
	DeleteGraph(frameImg_);
}

void Confirm::ChangeResult(RESULT result)
{
	result_ = result;
}

void Confirm::ChangeSelect(SELECT select)
{
	currentSelect_ = select;

	switch (select)
	{
	case SELECT::NONE:
		currentSelectPos_ = { 0, -100 };
		break;
	case SELECT::YES:
		currentSelectPos_ = { YES_POS_X, YES_POS_Y };
		break;
	case SELECT::NO:
		currentSelectPos_ = { NO_POS_X, NO_POS_Y };
		break;
	default:
		break;
	}
}

void Confirm::UpdateYes(void)
{
	switch (result_)
	{
	case RESULT::QUIT:
		Application::GetInstance()->SetEnd(true);
		break;
	case RESULT::MAIN_MENU:
		SceneManager::GetInstance()->JumpScene(std::make_shared<MainMenu>());
		break;
	default:
		break;
	}
}

void Confirm::UpdateNo(void)
{
	// 確認シーンを閉じる（ポーズシーンへ）
	SceneManager::GetInstance()->PopScene();

}
