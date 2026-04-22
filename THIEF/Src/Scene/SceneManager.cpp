#include "SceneManager.h"

#include <DxLib.h>

#include "Loading/Loading.h"
#include "TitleScene/TitleScene.h"
#include "GameScene/GameScene.h"

#include "../System/SystemManager.h"

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

	// 3D情報の初期化
	Init3D();

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

	// 正面から斜め下に向かったライト
	ChangeLightTypeDir({ 0.00f, -1.00f, 1.00f });
}

// 更新
void SceneManager::Update(void)
{
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
		// 積まれているもの全てを描画する
		for (auto& scene : scenes_)
		{
			// シーンの描画
			scene->Draw();
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

	// ロード画面の削除
	load_->Release();
	delete load_;
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

	// 新しく積む
	scenes_.push_back(scene);

	// 読み込み(非同期)
	load_->StartAsyncLoad();
	scenes_.back()->Load();
	load_->EndAsyncLoad();
}

