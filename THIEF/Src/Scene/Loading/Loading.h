#pragma once

#include "../../Common/Math/Vector2.h"

class Loading
{
public:

	// 現在描画している画像の種類
	enum NOW_TYPE
	{
		LOADING_0,
		LOADING_1,
		LOADING_2,
		LOADING_3,

		MAX,
	};

public:

	Loading(void);			// コンストラクタ

	void Init(void);		// 初期化
	void Load(void);		// 読み込み
	void Update(void);		// 更新
	void Draw(void);		// 描画
	void Release(void);		// 解放

public:

	// 非同期ロードの開始
	void StartAsyncLoad(void);	
	// 非同期ロードの終了
	void EndAsyncLoad(void);	

	// ロード中かを返す
	bool IsLoading(void) { return isLoading_; }	

private:
	
	// 最低でもロード画面を表示する時間
	static constexpr int MIN_LOAD_TIME = 60;	

	// 分割数縦横
	static constexpr int DIV_NUM_XY = 2;	

private:

	// 画像ハンドル
	int handles_[static_cast<int>(NOW_TYPE::MAX)];	

	// 座標
	Vector2 pos_ = { 0.0f,0.0f };

	// 現在描画している画像の種類
	NOW_TYPE nowType_ = NOW_TYPE::MAX;	

	// ロード中の判定用
	bool isLoading_ = false;	
	
	// 最低でもロード画面を表示する時間の範囲
	int loadTimer_ = 0;	
};
