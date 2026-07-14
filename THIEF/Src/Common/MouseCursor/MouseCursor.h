#pragma once
class MouseCursor
{
public:

	// コンストラクタ
	MouseCursor(void);
	// デストラクタ
	~MouseCursor(void);

	// 初期化処理
	void Init(void);
	// 更新処理
	void Update(void);
	// 描画処理
	void Draw(void);
	// 解放処理
	void Release(void);

private:

	// マウス画像のサイズ
	static constexpr int MOUSE_IMG_SIZE_WID = 0;
	static constexpr int MOUSE_IMG_SIZE_HIG = 0;

	// マウス画像
	int mouseImg_;

};

