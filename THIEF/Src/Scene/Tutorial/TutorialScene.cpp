#include <fstream>
#include <sstream>
#include "../../Application.h"
#include "../../Common/Manager/Input/InputManager.h"
#include "../SceneManager.h"
#include "../Pause/Pause.h"
#include "../GameScene/GameScene.h"
#include "../MainMenu/MainMenu.h"
#include "../../Common/Manager/Audio/AudioManager.h"
#include "../../Common/Manager/PlayerActionCounter/PlayerActionCounter.h"
#include "TutorialScene.h"

TutorialScene::TutorialScene(void)
{
	// 状態の登録
	stateTable_[Tutorial::MOVE] = &TutorialScene::Move;
	stateTable_[Tutorial::JUMP] = &TutorialScene::Jump;
	stateTable_[Tutorial::DASH] = &TutorialScene::Dash;
	stateTable_[Tutorial::CROUCH] = &TutorialScene::Crouch;
	stateTable_[Tutorial::SLIDING] = &TutorialScene::Sliding;
	stateTable_[Tutorial::LIGHT] = &TutorialScene::Light;
	stateTable_[Tutorial::MAP] = &TutorialScene::Map;
	stateTable_[Tutorial::GRAB] = &TutorialScene::Grab;
	stateTable_[Tutorial::RANGE] = &TutorialScene::Range;
	stateTable_[Tutorial::CART] = &TutorialScene::Cart;
	stateTable_[Tutorial::DELIVER] = &TutorialScene::Deliver;
}

TutorialScene::~TutorialScene(void)
{
}

void TutorialScene::Init(void)
{
	// 現在の確認項目を移動に設定
	currentState_ = Tutorial::MOVE;

	// 確認項目のクリアフラグ初期化
	isClearState_ = false;

	// クリアカウント初期化
	clearStateEndCount_ = 0;

	// プレイヤーの行動カウンタクラスの初期化
	PlayerActionCounter::GetInstance()->Init();
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

	// 確認項目がクリア判定になっていたら「Good job!」のための時間を取る
	if (isClearState_)
	{
		Clear();
	}
	else
	{
		if (stateTable_[currentState_])
		{
			(this->*stateTable_[currentState_])();
		}
	}

	//　ステップ10まで終わったら遷移する
	if (currentState_ == Tutorial::MAX)
	{
		currentState_ = Tutorial::MOVE;
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<MainMenu>());
		return;
	}
}

void TutorialScene::Draw(void)
{
#ifdef _DEBUG
	if (isClearState_)
	{
		DrawStringToHandle(150, 500, "Good job!", 0xffffff, Application::GetInstance()->GetFont());
	}
	// CLEAR以外の時は、CSVから読み込む
	else
	{
		int index = static_cast<int>(currentState_);
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
		Application::GetInstance()->GetFont(), "パーセント：　%.2f ％", PlayerActionCounter::GetInstance()->GetCounter(currentState_));
	// ステップ表示  ステートが0から始まるため、+1で補正
	DrawFormatStringToHandle(10, 270, 0xffffff,
		Application::GetInstance()->GetFont(), "ステップ：　%d / %d", static_cast<int>(currentState_) + 1, static_cast<int>(Tutorial::STATE::MAX));
#endif //_DEBUG
}

void TutorialScene::Release(void)
{
	if (!steps_.empty())
	{
		steps_.clear();
	}
}

void TutorialScene::SetState(Tutorial::STATE newState)
{
	// 指定されたステートへ変更
	currentState_ = newState;

	// 指定されたステートのカウントを初期化
	PlayerActionCounter::GetInstance()->ResetCounter(currentState_);

	// クリアカウント初期化
	clearStateEndCount_ = 0;

	// 確認項目クリアフラグを立てる
	isClearState_ = true;
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

		Tutorial::TutorialInfo data{};
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

	ifs.close();
}

void TutorialScene::Move(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_B))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	// TODO PlayerActionCounter::GetInstance()::GetMoveCount()
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をジャンプにする
		SetState(Tutorial::STATE::JUMP);
		return;
	}
#endif //_DEBUG

}

void TutorialScene::Jump(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をダッシュにする
		SetState(Tutorial::STATE::DASH);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Dash(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_B))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をしゃがみにする
		SetState(Tutorial::STATE::CROUCH);
		return;
	}
#endif //_DEBUG

}

void TutorialScene::Crouch(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_B))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をスライディングにする
		SetState(Tutorial::STATE::SLIDING);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Sliding(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をライトの動かし方にする
		SetState(Tutorial::STATE::LIGHT);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Light(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をマップの開き方にする
		SetState(Tutorial::STATE::MAP);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Map(void)
{
#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目を掴み方にする
		SetState(Tutorial::STATE::GRAB);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Grab(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をアイテムの動かし方にする
		SetState(Tutorial::STATE::RANGE);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Range(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をカートへの入れ方にする
		SetState(Tutorial::STATE::CART);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Cart(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目を納品の仕方にする
		SetState(Tutorial::STATE::DELIVER);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Deliver(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_B))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をアイテムの動かし方にする
		SetState(Tutorial::STATE::MAX);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Clear(void)
{
	// 規定値に到達したら
	if (clearStateEndCount_ >= MAX_CLEAR_COUNT)
	{
		// クリア判定を切る
		isClearState_ = false;
		return;
	}

	// カウントを進める
	clearStateEndCount_++;
}