#pragma once
#include "../Math/Vector2.h"

class MouseCursor
{
public:

	// マウス画像のサイズ
	static constexpr int MOUSE_IMG_SIZE_WID = 30;
	static constexpr int MOUSE_IMG_SIZE_HIG = 40;

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static MouseCursor& GetInstance(void);

	// 読み込み処理
	void Load(void);
	// 初期化処理
	void Init(void);
	// 更新処理
	void Update(void);
	// 描画処理
	void Draw(void);
	// 解放処理
	void Destroy(void);

	// マウスの描画フラグを設定
	void SetMouseDraw(bool flg);

private:

	// マウス画像
	int mouseImg_ = -1;

	// マウス表示フラグ　true / 表示,false / 非表示
	bool mouseDrawFlg_;

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

