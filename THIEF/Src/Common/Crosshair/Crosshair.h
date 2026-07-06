#pragma once

class PlayerController;

// 表示種類
enum CROSSHAIR_TYPE
{
	NOT_GRAB,
	CAN_GRAB,
	GRABBING,

	MAX,
};

class Crosshair
{
public:

	// コンストラクタ
	Crosshair(void);
	// デストラクタ
	~Crosshair(void);

	// 更新処理
	void Load(void);
	// 初期化処理
	void Init(void);
	// 更新処理
	void Update(void);
	// 描画処理
	void Draw(void);
	// 解放処理
	void Release(void);

	// クロスヘアの種類を変更
	void ChangeCrosshair(const CROSSHAIR_TYPE type);

private:

	// 画像ハンドル
	int img[CROSSHAIR_TYPE::MAX];

	// 表示中の種類
	CROSSHAIR_TYPE type_;
};

