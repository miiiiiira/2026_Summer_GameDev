#pragma once

#include <DxLib.h>

class Shader
{
public:

	// シェーダーのパラメータを保持する構造体
	struct Ctr
	{
		float scanlineIntensity;	// 走査線の濃さ
		float vignettePower;		// ビネットの鋭さ
		float glitchAmount;			// グリッチの強度　0.0で通常、1.0で崩壊
		float timer;				// ノイズやグリッチを動かす時間
		float curvatureAmount;		// 歪み度
		float noisePower;			// ノイズの強度
		float rgbShift;				// 色のずれ
		float glitchProbability;	// グリッチの発生率
	};

	// デフォルト値の初期化
	static constexpr Ctr DEFAULT_CTR = 
				{ 0.1f, 0.5f, 0.0f, 0.0f, 0.01f, 0.1f, 0.002f, 0.8f };

	void Init(void);			// 初期化

	void Draw(int texture);		// 描画
	
	void Release(void);			// 解放

	// パラメータ変更のセッター
	// 走査線の濃さの変更
	void SetScanlineIntensity(float val) { targetCtrParam_.scanlineIntensity = val; }

	// ビネットの鋭さの変更
	void SetVignettePower(float val) { targetCtrParam_.vignettePower = val; }

	// グリッチの強度の変更
	void SetGlitchAmount(float val) { targetCtrParam_.glitchAmount = val; }

	// 歪み度の変更
	void SetCurvatureAmount(float val, bool isLerpActive = true);

	// ノイズの強度の変更
	void SetNoisePower(float val) { targetCtrParam_.noisePower = val; }

	// 色のずれの変更
	void SetRgbShift(float val) { targetCtrParam_.rgbShift = val; }

	// グリッチの発生率の変更
	void SetGlitchProbability(float val, bool isLerpActive = true);

	// 全ての目標値をデフォルトに戻す
	void ResetParameters(void) { targetCtrParam_ = DEFAULT_CTR; }

	// パラメータのゲッター
	const Ctr& GetParameters(void) const { return currentCtrParam_;}

	// 現在のパラメータがデフォルト値かどうかを判定する
	bool IsDefault(void) const;

private:

	// 頂点数
	static constexpr int SQUARE_VERTEX_NUM = 4;

	// インデックス数
	static constexpr int SQUARE_INDEX_NUM = 6;

	// 補間の速さ(値が大きいほど素早く目標値に到達)
	static constexpr float LERP_SPEED = 0.05f;

	// シェーダの目標値と現在地を保持する構造体
	Ctr targetCtrParam_;	// 目標値

	Ctr currentCtrParam_;	// 現在値

	// 頂点情報
	// 頂点座標、色、テクスチャ座標を保持する構造体
	VERTEX2DSHADER vertex_[SQUARE_VERTEX_NUM];

	// 頂点インデックス
	WORD index_[SQUARE_INDEX_NUM];

	// ピクセルシェーダーのハンドル
	int psCtr_;

	// ピクセルシェーダー用の定数バッファのハンドル
	int psCtrConstBuf_;

	// 描画用の四角頂点を作成
	void MakeSquereVertex(void);

	// パラメータを補間する
	void UpdateParam(float& current, float target);
};