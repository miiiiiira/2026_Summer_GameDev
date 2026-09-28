#pragma once

#include "../Math/Vector2.h"

class MouseCursor
{
public:

	// シングルトン（生成・取得・削除）
	static void  CreateInstance(void) { if (instance_ == nullptr) { instance_ = new MouseCursor(); } }
	static MouseCursor* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; } }

private:

	// 静的インスタンス
	static MouseCursor* instance_;

	// コピー・ムーブ操作を禁止
	MouseCursor(const MouseCursor&) = delete;
	MouseCursor& operator=(const MouseCursor&) = delete;
	MouseCursor(MouseCursor&&) = delete;
	MouseCursor& operator=(MouseCursor&&) = delete;

public:

	// マウス画像のサイズ
	static constexpr int MOUSE_IMG_SIZE_WID = 30;	
	static constexpr int MOUSE_IMG_SIZE_HIG = 40;

public:

	MouseCursor(void);		// コンストラクタ

	void Load(void);	// 読み込み
	void Init(void);	// 初期化
	void Update(void);	// 更新
	void Draw(void);	// 描画
	void Destroy(void);	// 解放

public:

	// マウスの描画フラグを設定
	void SetMouseDraw(bool flg) { mouseDrawFlg_ = flg; }

private:

	// デバッグ表示
	void DebugDraw(void);	

private:

	// マウスの情報
	int mouseImg_ = -1;					// 画像ハンドル
	Vector2 mousePos_ = {0.0f,0.0f};	// 座標
	bool mouseDrawFlg_ = true;			// 表示フラグ　true / 表示,false / 非表示
};

