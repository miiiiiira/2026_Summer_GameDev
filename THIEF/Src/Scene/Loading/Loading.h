#pragma once

#include "../../Common/Math/Vector2.h"

class Loading
{
public:

	// 現在描画している画像の種類
	enum NOW_TYPE
	{
		Loading0,
		Loading1,
		Loading2,
		Loading3,

		MAX,
	};

	Loading();				// コンストラクタ

	void Init(void);		// 初期化
	void Load(void);		// 読み込み
	void Update(void);		// 更新
	void Draw(void);		// 描画
	void Release(void);		// 解放

	void StartAsyncLoad(void);	// 非同期ロードの開始
	void EndAsyncLoad(void);	// 非同期ロードの終了

	bool IsLoading(void) { return isLoading_; }	// ロード中かを返す

private:
	
	static constexpr int MIN_LOAD_TIME = 60;	// 最低でもロード画面を表示する時間

	static constexpr int DIV_NUM_XY = 2;	// 分割数縦横

private:

	int handles_[static_cast<int>(NOW_TYPE::MAX)];	// 画像ハンドル

	// 座標
	Vector2 pos_;	// X座標

	NOW_TYPE nowType_;	// 現在描画している画像の種類

	bool isLoading_;	// ロード中の判定用
	
	int loadTimer_;	// 最低でもロード画面を表示する時間の範囲
};
