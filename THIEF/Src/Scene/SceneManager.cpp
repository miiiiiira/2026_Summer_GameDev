#include "SceneManager.h"

#include "Loading/Loading.h"
#include "TitleScene/TitleScene.h"
#include "Shop/ShopScene.h"
#include "../Application.h"

#include "../Common/Manager/System/SystemManager.h"
#include "../Common/Manager/Score/ScoreManager.h"
#include "../Common/Manager/Input/InputManager.h"
#include "../Common/Manager/PlayerStatus/PlayerStatusManager.h"
#include "../Object/Component/PlayerController/Upgrade/UpgradeManager.h"
#include "../Common/Shader/Shader.h"

SceneManager* SceneManager::instance_ = nullptr;

SceneManager::SceneManager(void)
{
	scenes_.clear();
	load_ = nullptr;
	isGameEnd_ = false;
}

SceneManager::~SceneManager(void)
{
}

void SceneManager::Init(void)
{
	// ロード画面生成
	load_ = new Loading();
	load_->Init();
	load_->Load();

	//システム管理生成
	SystemManager::CreateInstance();

	//金額管理生成
	ScoreManager::CreateInstance();

	//プレイヤーのステータス管理生成
	PlayerStatusManager::CreateInstance();

	//アップグレード管理生成
	UpgradeManager::CreateInstance();

	// 3D情報の初期化
	Init3D();

	mainScreen_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, true);

	mTotalTime = 0.0f;
	mPreTime = std::chrono::system_clock::now();

	shader_ = new Shader();
	shader_->Init();

	isShader_ = true;

	// ゲームクリア判定用初期化
	isClear_ = false;
	// ゲームオーバー判定用初期化
	isOver_ = false;

	// 現在のステージを初期化
	currentStage_ = STAGE_NUM::STAGE_1;

	// 最初はタイトル画面から
	ChangeScene(std::make_shared<TitleScene>());

}

void SceneManager::Init3D(void)
{
	// 背景色設定
	SetBackgroundColor(0, 0, 0);

	// Zバッファを有効にする
	SetUseZBuffer3D(true);

	// Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(true);

	// バックカリングを有効にする
	SetUseBackCulling(true);

	// ライトの設定
	SetUseLighting(true);

	// Y軸のマイナス方向のディレクショナルライトに変更
	ChangeLightTypeDir({ 0.00f, -1.00f, 0.00f });

	// ディフューズカラー
	SetLightDifColor(DIF_COLOR);

	// フォグ設定
	SetFogEnable(true);
	// フォグの色
	SetFogColor(0, 0, 0);
	// フォグを発生させる奥行きの最小、最大距離
	SetFogStartEnd(FOG_START, FOG_END);
	// フォグの密度を設定
	SetFogDensity(0.3f);
}

// 更新
void SceneManager::Update(void)
{
	// デルタタイム
	auto nowTime = std::chrono::system_clock::now();
	mDeltaTime = static_cast<float>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - mPreTime).count() / 1000000000.0);
	mPreTime = nowTime;

	mTotalTime += mDeltaTime;

	// シーンがなければ終了
	if (scenes_.empty()) { return; }

	// ロード中
	if (load_->IsLoading())
	{
		// ロード更新
		load_->Update();

		// ロードの更新が終了していたら
		if (load_->IsLoading() == false)
		{
			// ロード後の初期化
			scenes_.back()->LoadEnd();
		}
	}
	// 通常の更新処理
	else
	{
		// デバイス切り替え処理
		SystemManager::GetInstance().Update();

		// 現在のシーンの更新
		scenes_.back()->Update();
	}
}

void SceneManager::Draw(void)
{
	// ロード中ならロード画面を描画
	if (load_->IsLoading())
	{
		// ロードの描画
		load_->Draw();
	}
	// 通常の更新
	else
	{
		SetDrawScreen(mainScreen_);
		ClearDrawScreen();

		// 積まれているもの全てを描画する
		for (auto& scene : scenes_)
		{
			// シーンの描画
			scene->Draw();
		}

		SetDrawScreen(DX_SCREEN_BACK);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

		if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_K))
		{
			isShader_ = !isShader_;
		}

		if (isShader_)
		{
			shader_->Draw(mainScreen_);
		}
		else
		{
			DrawGraph(0, 0, mainScreen_, false);
		}
	}
}

void SceneManager::Delete(void)
{
	// 全てのシーンの解放・削除
	for (auto& scene : scenes_) { scene->Release(); }
	scenes_.clear();

	// システム管理解放
	SystemManager::GetInstance().Destroy();

	// 金額管理解放
	ScoreManager::GetInstance().Destroy();

	// プレイヤーのステータス管理解放
	PlayerStatusManager::GetInstance().Destroy();

	// アップグレード管理解放
	UpgradeManager::GetInstance().Destroy();

	// ロード画面の削除
	load_->Release();
	delete load_;

	shader_->Release();
	delete shader_;
	DeleteGraph(mainScreen_);
}

void SceneManager::ChangeScene(std::shared_ptr<SceneBase>scene)
{
	// シーンが空か？
	if (scenes_.empty())
	{
		//空なので新しく入れる
		scenes_.push_back(scene);
	}
	else
	{
		//末尾のものを新しい物に入れ替える
		scenes_.back()->Release();
		scenes_.back() = scene;
	}

	// ゲームクリア判定用初期化
	isClear_ = false;
	// ゲームオーバー判定用初期化
	isOver_ = false;

	// 読み込み(非同期)
	load_->StartAsyncLoad();
	scenes_.back()->Load();
	load_->EndAsyncLoad();
}

void SceneManager::PushScene(std::shared_ptr<SceneBase> scene)
{
	//新しく積むのでもともと入っている奴はまだ削除されない
	scenes_.push_back(scene);
	scenes_.back()->Load();
	scenes_.back()->Init();
}

void SceneManager::PopScene(void)
{
	//積んであるものを消して、もともとあったものを末尾にする
	if (scenes_.size() > 0)
	{
		scenes_.back()->Release();
		scenes_.pop_back();
	}
}

void SceneManager::JumpScene(std::shared_ptr<SceneBase> scene)
{
	// 全て解放
	for (auto& scene : scenes_) { scene->Release(); }
	scenes_.clear();

	// ゲームクリア判定用初期化
	isClear_ = false;
	// ゲームオーバー判定用初期化
	isOver_ = false;

	// 新しく積む
	scenes_.push_back(scene);

	// 読み込み(非同期)
	load_->StartAsyncLoad();
	scenes_.back()->Load();
	load_->EndAsyncLoad();
}

const float& SceneManager::GetTotalTime(void)
{
	// TODO: return ステートメントをここに挿入します
	return mTotalTime;
}

const float& SceneManager::GetDeltaTime(void)
{
	// TODO: return ステートメントをここに挿入します
	return mDeltaTime;
}

void SceneManager::TrueGameClear(void)
{
	// フラグを立てる
	isClear_ = true;

	// 現在のステージを初期化
	currentStage_ = STAGE_NUM::STAGE_1;
}

void SceneManager::TrueGameOver(void)
{
	// フラグを立てる
	isOver_ = true;

	// 現在のステージを初期化
	currentStage_ = STAGE_NUM::STAGE_1;
}
