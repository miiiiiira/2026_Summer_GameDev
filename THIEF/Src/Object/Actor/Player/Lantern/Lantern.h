#pragma once

class Player;

class Lantern
{
public:

	// 大きさ
	static constexpr VECTOR SCALE = { 0.0f,0.0f,0.0f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f,0.0f,0.0f };

	// 座標
	static constexpr VECTOR DEFAULT_POS = { 0.0f,0.0f,0.0f };

	// ランタンの範囲
	static constexpr float LANTEERN_RANGE = 400.0f;

	// プレイヤーとランタンの相対座標
	static constexpr VECTOR REACH_DEFAULT_LIGHT = { 50.0f,-150.0f,150.0f };
	static constexpr VECTOR REACH_MAX_LIGHT = { 50.0f,-150.0f,350.0f };

	// 線形補間の係数
	static constexpr float COEFFICIENT = 0.1f;

	// コンストラクタ
	Lantern(Player* player);
	// デストラクタ
	~Lantern(void);

	// 読み込み処理
	void Load(void);
	// 初期化処理
	void Init(void);	
	// 更新処理
	void Update(const VECTOR& cameraPos,const VECTOR& cameraAngle);
	// 描画処理
	void Draw(void);	
	// 解放処理
	void Release(void);

private:
	// プレイヤーのポインタ
	Player* player_;

	// ポイントライトのハンドル
	int pointLightHandle_;

	// モデルのハンドル
	int modelId_;

	// モデルの大きさ
	VECTOR scale_;

	// モデルの向き
	VECTOR angle_;

	// モデルの座標
	VECTOR pos_;

	void UpdatePos(const VECTOR& cameraPos, const VECTOR& cameraAngle);

	// デバッグ用の描画処理
	void DebugDraw(void);
};

