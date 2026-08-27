#pragma once

#include "../SceneBase.h"
#include "../../Application.h"
#include <vector>

class GameOver : public SceneBase
{
public:

	enum class STATE
	{
		NONE,	// 動かない
		SHAKE,	// 画像揺らし
		CRACK,	// ひび割れ
		SELECT,	// 選択
		MAX,
	};

	// 選択ボタンの種類
	enum TYPE
	{
		RETRY,			// リトライ
		RETURN_TITLE,	// タイトルへ戻る
		NONE,
	};

	struct IMG_INFO
	{
		TYPE type;
		int graphHandle;
		int x, y;
		int sizeX, sizeY;
	};

	GameOver(void);					// コンストラクタ
	~GameOver(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:

	// 選択処理
	void SelectUpdate(void);

	// マウス選択
	void MouseSelect(void);

	// パッド選択
	void PadSelect(void);

	// ヒットストップカウンタが0じゃない場合に揺らし量を計算
	void GetShakeOffset(int& offset);

	void ChangeState(STATE state);	// 指定のステートに変更

	// チェンジステートが呼ばれた際の初期化処理
	void ChangeNone(void);
	void ChangeShake(void);
	void ChangeCrack(void);
	void ChangeSelect(void);

	// ステートごとの更新処理
	void UpdateNone(void);
	void UpdateShake(void);
	void UpdateCrack(void);
	void UpdateSelect(void);

private:

	// RETRY画像サイズ
	static constexpr int RETRY_SIZE_X = 152;
	static constexpr int RETRY_SIZE_Y = 32;

	// RETRY
	static constexpr int RETRY_POS_X = Application::SCREEN_SIZE_X / 2 - RETRY_SIZE_X / 2;
	static constexpr int RETRY_POS_Y = 485;

	// RETURN_TITLE画像サイズ
	static constexpr int RETURN_TITLE_SIZE_X = 152;
	static constexpr int RETURN_TITLE_SIZE_Y = 32;

	// RETURN_TITLE
	static constexpr int RETURN_TITLE_POS_X = Application::SCREEN_SIZE_X / 2 - RETURN_TITLE_SIZE_X / 2;
	static constexpr int RETURN_TITLE_POS_Y = 550;

	static constexpr int FRAME_OFFSET = 10;	// フレームのオフセット

	static constexpr int SHAKE_TIME = 30;	// 揺らす時間

	// 透明度
	static constexpr int ADD_ALPHA = 3;		// 加算値
	static constexpr int ALPHA_MAX = 255;	// 最大値

	// ステップ数
	static constexpr float NON_STEP = 1.5f;		// ステートNON時
	static constexpr float CRACK_STEP = 1.0f;	// ステートCrack時

private:

	// 画像ハンドル
	int handle_ = -1;		// ゲームオーバー画像
	int crackHandle_ = -1;	// ひび割れ画像

	std::vector<IMG_INFO> buttons_;		// ボタンの情報を格納する配列
	
	STATE state_;	// 現在のステート

	float step_ = 0; // 次の処理を行うまでの時間

	int alpha_ = 0;	// アルファ値(ボタン表示に使用)

	TYPE currentType_;	// 現在選択しているメニュー

	int hitStopCounter_ = 0;	// ヒットストップ用のカウンター
};

