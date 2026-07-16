#include <fstream>
#include <sstream>
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
	stateTable_[DUSH] = &TutorialScene::Dush;
	stateTable_[CROUCH] = &TutorialScene::Crouch;
	stateTable_[SLIDING] = &TutorialScene::Sliding;
	stateTable_[LIGHT] = &TutorialScene::Light;
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

	if (currentPlayCount_ == totalPlayCount_)
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
	switch (currentState_)
	{
	case STATE::CLEAR:
		DrawString(200, 450, "good job!", 0xffffff);
	}

	DrawFormatString(10, 250, 0xffffff, "パーセント： %.2f ％", currentStepValue_);
	DrawFormatString(10, 270, 0xffffff, "現在のステップ：%d / %d", currentPlayCount_, totalPlayCount_);
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

		if (strSplit.size() != 4)
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
		data.text = strSplit[index++];

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

void TutorialScene::Dush(void)
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
