#pragma once
#include <DxLib.h>

class Shader
{
public:

	// シェーダーのパラメータを保持する構造体
	struct CrtParam
	{
		float scanlineIntensity = 0.0f;	// 走査線の濃さ
		float vignettePower = 0.0f;		// ビネットの鋭さ
		float glitchAmount = 0.0f;		// グリッチの強度　0.0で通常、1.0で崩壊
		float timer = 0.0f;				// ノイズやグリッチを動かす時間
		float curvatureAmount = 0.0f;	// 歪み度
		float noisePower = 0.0f;		// ノイズの強度
		float rgbShift = 0.0f;			// 色のずれ
		float padding = 0.0f;			// 定数バッファを16バイト境界に揃えるため
	};

	// デフォルト値の初期化
	static constexpr float DEFAULT_SCANLINE_INTENSITY = 0.1f;	// 走査線の濃さ
	static constexpr float DEFAULT_VIGNETTE_POWER = 0.5f;		// ビネットの鋭さ
	static constexpr float DEFAULT_GLITCH_AMOUNT = 0.0f;		// グリッチの強度　0.0で通常、1.0で崩壊
	static constexpr float DEFAULT_TIMER = 0.0f;				// ノイズやグリッチを動かす時間
	static constexpr float DEFAULT_CURVATURE_AMOUNT = 0.01f;	// 歪み度
	static constexpr float DEFAULT_NOISE_POWER = 0.1f;			// ノイズの強度
	static constexpr float DEFAULT_RGB_SHIFT = 0.002f;			// 色のずれ

	// CRTシェーダーの初期値
	static constexpr CrtParam DEFAULT_CRT = { 
					DEFAULT_SCANLINE_INTENSITY, 
					DEFAULT_VIGNETTE_POWER, 
					DEFAULT_GLITCH_AMOUNT, 
					DEFAULT_TIMER, 
					DEFAULT_CURVATURE_AMOUNT, 
					DEFAULT_NOISE_POWER, 
					DEFAULT_RGB_SHIFT, 
					0.0f		// padding
	};

	// ダメージ時のビネットの鋭さ
	static constexpr float DAMAGE_VIGNETTE_POWER = 0.7f;

	// メニュー・UI画面共通のシェーダ設定値
	// 走査線の濃さ
	static constexpr float MENU_SCANLINE_INTENSITY = 0.5f;
	// グリッチで横にずらす最大値
	static constexpr float MENU_GLITCH_AMOUNT = 0.005f;
	// 湾曲の強さ
	static constexpr float MENU_CURVATURE_AMOUNT = 0.4f;
	// ノイズの強さ
	static constexpr float MENU_NOISE_POWER = 0.5f;
	// RGBの色ずれ幅
	static constexpr float MENU_RGB_SHIFT = 0.004f;

	// ゲームオーバー用の歪みの強さ
	static constexpr float GAMEOVER_CURVATURE_AMOUNT = 0.1f;

	// コンストラクタ
	Shader(void);

	// デストラクタ
	~Shader(void);

	// 初期化
	void Init(void);

	// 描画
	void Draw(int texture);

	// 解放
	void Release(void);

	// パラメータ変更のセッター
	// 走査線の濃さの変更
	void SetScanlineIntensity(float val) { targetCrtParam_.scanlineIntensity = val; }

	// ビネットの鋭さの変更
	void SetVignettePower(float val) { targetCrtParam_.vignettePower = val; }

	// グリッチの強度の変更
	void SetGlitchAmount(float val) { targetCrtParam_.glitchAmount = val; }

	// 歪み度の変更（補間なしで即座に切り替えたい場面があるため isLerpActive を用意）
	void SetCurvatureAmount(float val, bool isLerpActive = true);

	// ノイズの強度の変更
	void SetNoisePower(float val) { targetCrtParam_.noisePower = val; }

	// 色のずれの変更
	void SetRgbShift(float val) { targetCrtParam_.rgbShift = val; }

	// 全ての目標値をデフォルトに戻す
	void ResetParameters(void) { targetCrtParam_ = DEFAULT_CRT; }

	// 現在のパラメータを取得
	const CrtParam& GetParameters(void) const { return currentCrtParam_;}

	// 現在のパラメータがデフォルト値かどうかを判定する
	bool IsDefault(void) const;

private:

	// 頂点数
	static constexpr int SQUARE_VERTEX_NUM = 4;

	// インデックス数
	static constexpr int SQUARE_INDEX_NUM = 6;

	// 補間の速さ(値が大きいほど素早く目標値に到達)
	static constexpr float LERP_SPEED = 0.05f;

	// シェーダのパラメータ
	// 目標値、セッターで変更する
	CrtParam targetCrtParam_ = {};

	// 現在値、シェーダーへ渡す
	CrtParam currentCrtParam_ = {};

	// 頂点情報
	// 頂点座標、色、テクスチャ座標を保持する構造体
	VERTEX2DSHADER vertex_[SQUARE_VERTEX_NUM] = {};

	// 頂点インデックス
	WORD index_[SQUARE_INDEX_NUM] = {};

	// ピクセルシェーダーのハンドル
	int psCrt_ = -1;

	// ピクセルシェーダー用の定数バッファのハンドル
	int psCrtConstBuf_ = -1;

	// 描画用の四角頂点を作成
	void MakeSquareVertex(void);

	// パラメータを補間する
	void UpdateParam(float& current, float target);
};