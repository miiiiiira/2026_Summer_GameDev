#include "TitleScene.h"

#include <DxLib.h>

#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../../Application.h"
#include "../Confirm/Confirm.h"
#include "../GameScene/GameScene.h"
#include "../MainMenu/MainMenu.h"
#include "../../Common/Shader/Shader.h"
#include "../../Common/MouseCursor/MouseCursor.h"

TitleScene::TitleScene(void)
{
	handle_ = -1;
	buttonHandle_ = -1;
	alpha_ = 0.0f;
	isIncreasing_ = false;
	isPlaySoundSE_ = false;
	confirm_ = nullptr;
	// マウスの表示する
	MouseCursor::GetInstance()->SetMouseDraw(true);
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	alpha_ = 255.0f;
	isIncreasing_ = false;

	// シェーダーの初期化処理
	ShaderInit();
}

void TitleScene::Load(void)
{
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::TITLE);
	handle_ = LoadGraph("Data/Image/Common/Title.png");
	buttonHandle_ = LoadGraph("Data/Image/Title/PushAnyButton.png");

	confirm_ = std::make_shared<Confirm>();
}

void TitleScene::LoadEnd(void)
{
	Init();
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_TITLE);
}

void TitleScene::Update(void)
{
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::CANCEL) || InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::CANCEL))
	{
		confirm_->ChangeType(Confirm::TYPE::QUIT);
		SceneManager::GetInstance()->PushScene(confirm_);
		return;
	}

	// ボタンが押されると次のシーンへ
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::DECIDE) || InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::DECIDE))
	{
		if (!isPlaySoundSE_)
		{
			// ボタン音
			AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_2);
			alpha_ = 0.0f;
			isPlaySoundSE_ = true;
		}

		// シェーダをデフォルトに変更する
		SceneManager::GetInstance()->GetShader()->ResetParameters();
	}

	if (SceneManager::GetInstance()->GetShader()->IsDefault())
	{
		SceneManager::GetInstance()->PushScene(std::make_shared<MainMenu>());
		return;
	}

	// ボタンを押されたら動きを止める
	if (isPlaySoundSE_)return;

	// ボタンのアルファ値を変化させる
	if (isIncreasing_)
	{
		alpha_ += ALPHA_SPEED; // 増加速度

		// 増加が最大になったら減少に切り替える
		if(alpha_ >= ALPHA_MAX)
		{
			alpha_ = ALPHA_MAX;
			isIncreasing_ = false; 
		}
	}
	else
	{
		alpha_ -= ALPHA_SPEED; // 減少速度

		// 減少が最小になったら増加に切り替える
		if(alpha_ <= ALPHA_MIN)
		{
			alpha_ = ALPHA_MIN;
			isIncreasing_ = true;
		}
	}
}

void TitleScene::Draw(void)
{
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 1.0, 0.0, handle_, true);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha_));
	DrawRotaGraph(BUTTON_POS_X, BUTTON_POS_Y, 1.0, 0.0, buttonHandle_, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

void TitleScene::Release(void)
{
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::TITLE);

	DeleteGraph(handle_);
	DeleteGraph(buttonHandle_);
}

void TitleScene::ShaderInit(void)
{

	// 走査線
	SceneManager::GetInstance()->GetShader()->SetScanlineIntensity(0.5f);
	// グリッチ
	SceneManager::GetInstance()->GetShader()->SetGlitchAmount(0.005f);
	// 歪み
	SceneManager::GetInstance()->GetShader()->SetCurvatureAmount(0.4f, false);
	// ノイズ
	SceneManager::GetInstance()->GetShader()->SetNoisePower(0.5f);
	// 色ずれ
	SceneManager::GetInstance()->GetShader()->SetRgbShift(0.004f);
}
