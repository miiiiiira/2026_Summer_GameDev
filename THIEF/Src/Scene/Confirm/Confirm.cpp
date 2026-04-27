#include <vector>

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
	frameImg_ = -1;
}

Confirm::~Confirm(void)
{
}

void Confirm::Init(void)
{
	ChangeSelect(SELECT::NONE);

	switch (confirmType_)
	{
	case TYPE::QUIT:
		confirmImg_ = quitImg_;
		break;
	case TYPE::MAIN_MENU:
		confirmImg_ = mainMenuImg_;
		break;
	}
}

void Confirm::Load(void)
{
	quitImg_ = LoadGraph((Application::PATH_IMAGE + "ConfirmQuit.png").c_str());			// QUITの時の確認画面
	mainMenuImg_ = LoadGraph((Application::PATH_IMAGE + "ConfirmMainMenu.png").c_str());	// MAIN MENUの時の確認画面
	frameImg_ = LoadGraph((Application::PATH_IMAGE + "frame.png").c_str());					// フレーム画像
	// 配列をクリアにしてから、画像を追加
	selectButtons_.clear();
	// YES画像
	selectButtons_.push_back({ SELECT::YES, LoadGraph((Application::PATH_IMAGE + "Yes.png").c_str()),
								YES_POS_X, YES_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// NO画像
	selectButtons_.push_back({ SELECT::NO,  LoadGraph((Application::PATH_IMAGE + "No.png").c_str()),
							NO_POS_X, NO_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
}

void Confirm::LoadEnd(void)
{
	Init();
}

void Confirm::Update(void)
{
	// 衝突判定
	SELECT nextSelect = SELECT::NONE;

	for (const auto& button : selectButtons_)
	{
		if (Collision::HitMouse2Box({ static_cast<float>(button.x), static_cast<float>(button.y) },
			static_cast<float>(button.sizeX), static_cast<float>(button.sizeY)))
		{
			nextSelect = button.type;
			break;
		}
	}

	ChangeSelect(nextSelect);

	// マウスを左クリックされなかったら、処理しない
	if (!InputManager::GetInstance()->IsClickMouseLeft()) return;
	// どの選択肢も選ばれていない場合は処理しない
	if (currentSelect_ == SELECT::NONE) return;

	switch (currentSelect_)
	{
	case Confirm::SELECT::YES:
		UpdateYes();
		break;
	case Confirm::SELECT::NO:
		UpdateNo();
		break;
	}
}

void Confirm::Draw(void)
{
	DrawGraph(CONFIRM_POS_X, CONFIRM_POS_Y, confirmImg_, true);				// 確認の文字を表示

	for (const auto& button : selectButtons_)
	{
		if (button.type == currentSelect_)
		{
			DrawExtendGraph(button.x, button.y,
				button.x + button.sizeX,
				button.y + button.sizeY,
				frameImg_, true);
		}
		DrawGraph(button.x, button.y, button.graphHandle, true);
	}
}

void Confirm::Release(void)
{
	for (const auto& button : selectButtons_)
	{
		DeleteGraph(button.graphHandle);
	}
	selectButtons_.clear();

	DeleteGraph(frameImg_);
	frameImg_ = -1;
	DeleteGraph(mainMenuImg_);
	mainMenuImg_ = -1;
	DeleteGraph(quitImg_);
	quitImg_ = -1;
	confirmImg_ = -1;

}

void Confirm::ChangeType(TYPE type)
{
	confirmType_ = type;
}

void Confirm::ChangeSelect(SELECT select)
{
	currentSelect_ = select;
}

void Confirm::UpdateYes(void)
{
	switch (confirmType_)
	{
	case TYPE::QUIT:
		Application::GetInstance()->SetEnd(true);
		break;
	case TYPE::MAIN_MENU:
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