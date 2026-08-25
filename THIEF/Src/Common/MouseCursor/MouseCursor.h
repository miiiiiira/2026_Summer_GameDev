#pragma once
#include "../Math/Vector2.h"

class MouseCursor
{
public:

	static constexpr int MOUSE_IMG_SIZE_WID = 30;	// マウス画像の横サイズ
	static constexpr int MOUSE_IMG_SIZE_HIG = 40;	// マウス画像の縦サイズ

public:

	// シングルトン（生成・取得・削除）
	static void  CreateInstance(void) { if (instance_ == nullptr) { instance_ = new MouseCursor(); } }
	static MouseCursor* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; } }

public:

	MouseCursor(void);		// コンストラクタ

	void Load(void);	// 読み込み
	void Init(void);	// 初期化
	void Update(void);	// 更新
	void Draw(void);	// 描画
	void Destroy(void);	// 解放
	
	void SetMouseDraw(bool flg);	// マウスの描画フラグを設定

private:

	static MouseCursor* instance_;	// 静的インスタンス

	// コピー・ムーブ操作を禁止
	MouseCursor(const MouseCursor&) = delete;
	MouseCursor& operator=(const MouseCursor&) = delete;
	MouseCursor(MouseCursor&&) = delete;
	MouseCursor& operator=(MouseCursor&&) = delete;

private:

	void DebugDraw(void);	// デバッグ表示

private:

	int mouseImg_ = -1;	// マウス画像

	Vector2 mousePos_;	// マウス座標

	bool mouseDrawFlg_ = true;	// マウス表示フラグ　true / 表示,false / 非表示
};

