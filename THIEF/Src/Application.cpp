#include "Application.h"

#include <DxLib.h>

#include "Common/System/FpsControl.h"
#include "Common/Manager/Input/InputManager.h"
#include "Scene/SceneManager.h"
#include "Common/Manager/Audio/AudioManager.h"

Application* Application::instance_ = nullptr;

const std::string Application::PATH_MODEL = "Data/Model/";
const std::string Application::PATH_IMAGE= "Data/Image/";

Application::Application(void)
{
	isInitFail_ = false;
	isReleaseFail_ = false;
	fps_ = nullptr;
}

Application::~Application(void)
{
}

void Application::Init(void)
{

	// アプリケーションの初期設定
	SetWindowText("THIEF");

	// ウィンドウ関連
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);

	ChangeWindowMode(false);

#ifdef _DEBUG
	ChangeWindowMode(true);
#endif // _DEBUG


	// DxLibの初期化
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	isInitFail_ = false;
	if (DxLib_Init() == -1)
	{
		isInitFail_ = true;
		return;
	}

	// 描画先画面を裏にする
	SetDrawScreen(DX_SCREEN_BACK);

	// キー制御初期化
	SetUseDirectInputFlag(true);
	InputManager::CreateInstance();
	InputManager::GetInstance()->Init();

	// サウンド
	AudioManager::CreateInstance();
	AudioManager::GetInstance()->Init();
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::SYSTEM);

	// シーン管理初期化
	SceneManager::CreateInstance();
	SceneManager::GetInstance()->Init();

	// FPS初期化
	fps_ = new FpsControl;
	fps_->Init();

	// フォントの追加
		font_ = CreateFontToHandle("Shikakufuto_Free", FONT_SIZE, 1, DX_FONTTYPE_ANTIALIASING);
}

// ゲームループ
void Application::Run(void)
{
	// ゲームループ
	while (ProcessMessage() == 0 && !isEnd_)
	{
		// フレームレート更新
		// 1/60秒経過していないなら再ループさせる
		if (!fps_->UpdateFrameRate()) continue;

		// 画面を初期化
		ClearDrawScreen();

		InputManager::GetInstance()->Update();	// 入力制御更新
		SceneManager::GetInstance()->Update();	// シーン管理更新
		SceneManager::GetInstance()->Draw();	// シーン管理描画

		fps_->CalcFrameRate();	// フレームレート計算
		fps_->DrawFrameRate();	// フレームレート描画

		ScreenFlip();
	}
}

void Application::Delete(void)
{
	// 入力制御削除
	InputManager::DeleteInstance();

	// シーン管理解放・削除
	SceneManager::GetInstance()->Delete();
	SceneManager::DeleteInstance();

	// サウンド削除
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::SYSTEM);
	AudioManager::GetInstance()->DeleteAll();
	AudioManager::DeleteInstance();

	// フレームレート解放
	delete fps_;

	// フォントの削除
	DeleteFontToHandle(font_);

	// DxLib終了
	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}
}

bool Application::IsInitFail(void) const
{
	return isInitFail_;
}

bool Application::IsReleaseFail(void) const
{
	return isReleaseFail_;
}

void Application::SetEnd(bool isEnd)
{
	isEnd_ = isEnd;
}

int Application::GetFont(void)
{
	return font_;
}
