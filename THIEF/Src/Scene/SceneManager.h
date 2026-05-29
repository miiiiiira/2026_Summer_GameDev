#pragma once

#include <list>
#include <memory>
#include <chrono>
#include <DxLib.h>

class SceneBase;
class Loading;
class Shader;

class SceneManager
{
public:
	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new SceneManager(); } };
	static SceneManager* GetInstance(void) { return instance_; };
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

private:

	// 静的インスタンス
	static SceneManager* instance_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SceneManager(void);
	// デストラクタも同様
	~SceneManager(void);

	// コピー・ムーブ操作を禁止
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

	// 下記をコンパイルエラーさせるため 上記を追加
	// SceneManager copy = *SceneManager::GetInstance();
	// SceneManager copied(*SceneManager::GetInstance());
	// SceneManager moved = std::move(*SceneManager::GetInstance());

public:

	// 環境光のカラー値、アルファ値
	static constexpr COLOR_F DIF_COLOR = { 0.8f, 0.8f, 0.8f, 1.0f };

	void Init(void);	// 初期化
	void Init3D(void);	// 3Dの初期化
	void Update(void);	// 更新
	void Draw(void);	// 描画
	void Delete(void);	// リソースの破棄

	// 状態遷移		遷移させたいシーン,全てのシーンを解放させるか
	void ChangeScene(std::shared_ptr<SceneBase> scene);

	// シーンを新しく積む
	void PushScene(std::shared_ptr<SceneBase> scene);

	// 最後に追加したシーンを削除する。
	void PopScene(void);

	// 強制的に特定のシーンに飛ぶ。リセットをかけ特定のシーンのみにする。
	void JumpScene(std::shared_ptr<SceneBase> scene);

	// ゲーム終了
	void GameEnd(void) { isGameEnd_ = true; }

	// ゲーム終了取得
	bool GetGameEnd(void) { return isGameEnd_; }

	const float& GetTotalTime(void);
	const float& GetDeltaTime(void);

	// ゲームクリアかどうか取得
	const bool GetIsClear(void) { return isClear_; }
	// ゲームオーバーかどうか取得
	const bool GetIsOver(void) { return isOver_; }

	// ゲームクリアのフラグをたてる
	void TrueGameClear(void);
	// ゲームオーバーのフラグをたてる
	void TrueGameOver(void);

private:

	// フォグのスタート位置終了位置
	static constexpr float FOG_START = 0.0f;
	static constexpr float FOG_END = 5000.0f;

	// 各種シーン
	std::list<std::shared_ptr<SceneBase>> scenes_;

	// ロード画面
	Loading* load_;

	// シェーダー
	Shader* shader_;

	// ゲーム終了
	bool isGameEnd_;

	// デルタタイム
	std::chrono::system_clock::time_point mPreTime;
	float mDeltaTime;
	float mTotalTime;

	int mainScreen_;
	bool isShader_;

	// ゲームクリア判定用
	bool isClear_;
	// ゲームオーバー判定用
	bool isOver_;
};
