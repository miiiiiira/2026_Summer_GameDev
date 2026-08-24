#pragma once
#include "../Math/Vector2.h"

class MouseCursor
{
public:

	// マウス画像のサイズ
	static constexpr int MOUSE_IMG_SIZE_WID = 30;
	static constexpr int MOUSE_IMG_SIZE_HIG = 40;

public:

	//シングルトン
	static void  CreateInstance(void) { if (instance_ == nullptr) { instance_ = new MouseCursor(); } }
	static MouseCursor* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; } }

	void Load(void);	// 読み込み
	void Init(void);	// 初期化
	void Update(void);	// 更新
	void Draw(void);	// 描画
	void Destroy(void);	// 解放
	
	void SetMouseDraw(bool flg);	// マウスの描画フラグを設定

private:

	// マウス画像
	int mouseImg_ = -1;

	// マウス表示フラグ　true / 表示,false / 非表示
	bool mouseDrawFlg_ = true;

	// マウス座標
	Vector2 mousePos_;

	// 静的インスタンス
	static MouseCursor* instance_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	MouseCursor(void);
	// コピーコンストラクタも同様
	MouseCursor(const MouseCursor& manager) = default;
	// デストラクタも同様
	~MouseCursor(void) = default;

	// デバッグ表示
	void DebugDraw(void);
};

