#include <fstream>
#include <sstream>
#include "../../Application.h"
#include "../../Common/Manager/Input/InputManager.h"
#include "../SceneManager.h"
#include "../Pause/Pause.h"
#include "../GameScene/GameScene.h"
#include "../../Common/Manager/Audio/AudioManager.h"
#include "TutorialScene.h"

TutorialScene::TutorialScene(void)
{
	// 状態の登録
	stateTable_[MOVE] = &TutorialScene::Move;
	stateTable_[JUMP] = &TutorialScene::Jump;
	stateTable_[DASH] = &TutorialScene::Dash;
	stateTable_[CROUCH] = &TutorialScene::Crouch;
	stateTable_[SLIDING] = &TutorialScene::Sliding;
	stateTable_[LIGHT] = &TutorialScene::Light;
	stateTable_[MAP] = &TutorialScene::Map;
	stateTable_[GRAB] = &TutorialScene::Grab;
	stateTable_[RANGE] = &TutorialScene::Range;
	stateTable_[CART] = &TutorialScene::Cart;
	stateTable_[DELIVER] = &TutorialScene::Deliver;
	stateTable_[CLEAR] = &TutorialScene::Clear;
}

TutorialScene::~TutorialScene(void)
{
}

void TutorialScene::Init(void)
{
	currentState_ = STATE::MOVE;
	nextState_ = STATE::MOVE;
	currentPlayCount_ = steps_[0].id;
	currentStepValue_ = 0.0f;
	totalPlayCount_ = static_cast<int>(STATE::MAX) - 2;
	
}

void TutorialScene::Load(void)
{
	LoadCsvData();
}

void TutorialScene::LoadEnd(void)
{
	Init();
}

void TutorialScene::Update(void)
{
	if (InputManager::GetInstance()->PauseButtons())
	{
		// ポーズ画面を開いたサウンド
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_PAUSE_ON);
		// ポーズモードへ
		SceneManager::GetInstance()->PushScene(std::make_shared<Pause>());
		return;
	}

	//　ステップ10まで終わったら遷移する
	if (currentPlayCount_ == totalPlayCount_ + 1)
	{
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameScene>());
		return;
	}

	if (currentState_ != STATE::CLEAR)
	{
		// 100％に達したら、クリアに遷移
		if (currentStepValue_ >= MAX_VALUE)
		{
			int nextVal = static_cast<int>(currentState_) + 1;
			nextState_ = static_cast<STATE>(nextVal);

			SetState(STATE::CLEAR);
			return;
		}		
	}

	if (stateTable_[currentState_])
	{
		(this->*stateTable_[currentState_])();
	}
}

void TutorialScene::Draw(void)
{
#ifdef _DEBUG
	if (currentState_ == STATE::CLEAR)
	{
		DrawStringToHandle(150, 500, "Good job!", 0xffffff, Application::GetInstance()->GetFont());
	}
	// CLEAR以外の時は、CSVから読み込む
	else
	{
		int index = currentPlayCount_ - 1;
		if (index >= 0 && index < static_cast<int>(steps_.size()))
		{
			// steps_ から取得し、描画
			// テキスト表示
			DrawStringToHandle(150, 500, steps_[index].textEN.c_str(), 0xffffff, Application::GetInstance()->GetFont());
			DrawStringToHandle(150, 540, steps_[index].textJP.c_str(), 0xffffff, Application::GetInstance()->GetDefaultFont());
			// ステート表示
			DrawFormatStringToHandle(10, 230, 0xffffff, 
					Application::GetInstance()->GetFont(), "ステート：　%s", steps_[index].type.c_str());
		}
	}
	// パーセント表示
	DrawFormatStringToHandle(10, 250, 0xffffff,
		Application::GetInstance()->GetFont(), "パーセント：　%.2f ％", currentStepValue_);
	// ステップ表示
	DrawFormatStringToHandle(10, 270, 0xffffff,
		Application::GetInstance()->GetFont(), "ステップ：　%d / %d", currentPlayCount_, totalPlayCount_);
#endif //_DEBUG
}

void TutorialScene::Release(void)
{
	if (!steps_.empty())
	{
		steps_.clear();
	}
}

void TutorialScene::SetState(STATE newState)
{
	currentState_ = newState;
	currentStepValue_ = 0.0f;
}

void TutorialScene::LoadCsvData(void)
{
	steps_.clear();

	// ファイルの読み込み
	std::ifstream ifs = std::ifstream("Data/Csv/TutorialData.csv");
	if (!ifs)
	{
#ifdef _DEBUG
		printf("ファイルが開けませんでした。");
#endif // _DEBUG
		return;	//	エラー発生
	}

	// ファイルを１行ずつ読み込む
	std::string line;// 1行の文字情報
	bool isHeader = true;

	while (getline(ifs, line))
	{
		if (isHeader)
		{
			isHeader = false;
			continue;
		}

		// １行をカンマ区切りで分割
		std::istringstream stream(line);
		std::string field;
		std::vector<std::string> strSplit; // 1行を1文字の動的配列に分割


		while (getline(stream, field, ','))
		{
			strSplit.push_back(field);
		}

		if (strSplit.size() != 5)
		{
			continue;
		}

		TutorialInfo data{};
		int index = 0;

		// ID
		data.id = std::stoi(strSplit[index++]);

		// タイプ
		data.type = strSplit[index++];

		// 値
		data.value = std::stof(strSplit[index++]);

		// テキスト
		data.textEN = strSplit[index++];
		data.textJP = strSplit[index++];

		steps_.push_back(data);
	}
}

void TutorialScene::Move(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_B))
	{
		currentStepValue_ += steps_[currentPlayCount_ - 1].value;
	}
#endif //_DEBUG

}

void TutorialScene::Jump(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		currentStepValue_ += steps_[currentPlayCount_ - 1].value;
	}
#endif //_DEBUG
}

void TutorialScene::Dash(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_B))
	{
		currentStepValue_ += steps_[currentPlayCount_ - 1].value;
	}
#endif //_DEBUG

}

void TutorialScene::Crouch(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_B))
	{
		currentStepValue_ += steps_[currentPlayCount_ - 1].value;
	}
#endif //_DEBUG
}

void TutorialScene::Sliding(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		currentStepValue_ += steps_[currentPlayCount_ - 1].value;
	}
#endif //_DEBUG
}

void TutorialScene::Light(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		currentStepValue_ += steps_[currentPlayCount_ - 1].value;
	}
#endif //_DEBUG
}

void TutorialScene::Map(void)
{
#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		currentStepValue_ += steps_[currentPlayCount_ - 1].value;
	}
#endif //_DEBUG
}

void TutorialScene::Grab(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		currentStepValue_ += steps_[currentPlayCount_ - 1].value;
	}
#endif //_DEBUG
}

void TutorialScene::Range(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		currentStepValue_ += steps_[currentPlayCount_ - 1].value;
	}
#endif //_DEBUG
}

void TutorialScene::Cart(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		currentStepValue_ += steps_[currentPlayCount_ - 1].value;
	}
#endif //_DEBUG
}

void TutorialScene::Deliver(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		currentStepValue_ += steps_[currentPlayCount_ - 1].value;
	}
#endif //_DEBUG
}

void TutorialScene::Clear(void)
{
	// 100以上になったらクリア
	if (currentStepValue_ >= MAX_VALUE)
	{
		currentPlayCount_++;

		if (currentState_ != STATE::DELIVER)
		{
			SetState(nextState_);
		}	 
		return;
	}

	// クリアステート時は、時間でカウントさせる
	currentStepValue_ += SceneManager::GetInstance()->GetDeltaTime() * 60.0f;

}