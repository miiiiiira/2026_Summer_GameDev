#pragma once

#include <DxLib.h>

class Shader
{
public:

	struct Ctr
	{
		float scanlineIntensity;      // 走査線の濃さ
		float vignettePower;          // ビネットの鋭さ
		float glitchAmount;           // グリッチの強度　0.0で通常、1.0で崩壊
		float timer;                  // ノイズやグリッチを動かす時間
		float curvatureAmount;        // 歪み度
		float noisePower;             // ノイズの強度
		float rgbShift;               // 色のずれ
		float dummy1;
	};

	// デフォルト値の初期化
	static constexpr Ctr DEFAULT_CTR = { 0.1f, 0.5f, 0.0f, 0.0f, 0.01f, 0.1f, 0.002f, 0.0f };

	void Init(void);
	void Draw(int texture);
	void Release(void);

	// パラメータ変更のセッター
	void SetScanlineIntensity(float val) { targetCtrParam_.scanlineIntensity = val; }
	void SetVignettePower(float val) { targetCtrParam_.vignettePower = val; }
	void SetGlitchAmount(float val) { targetCtrParam_.glitchAmount = val; }
	void SetCurvatureAmount(float val, bool isLerpActive = true);
	void SetNoisePower(float val) { targetCtrParam_.noisePower = val; }
	void SetRgbShift(float val) { targetCtrParam_.rgbShift = val; }

	// 全ての目標値をデフォルトに戻す
	void ResetParameters(void) { targetCtrParam_ = DEFAULT_CTR; }

	// パラメータのゲッター
	const Ctr& GetParameters(void) const { return currentCtrParam_;}

	bool IsDefault(void) const;

private:

	Ctr targetCtrParam_;    // 目標値
	Ctr currentCtrParam_;   // 現在値

	// 頂点情報
	VERTEX2DSHADER mVertex[4];
	WORD mIndex[6];
	int psCtr_;
	int psCtrConstBuf_;

	// 描画用の四角頂点を作成
	void MakeSquereVertex(void);

	void UpdateParam(float& current, float target);

	// 補間の速さ（値が大きいほど素早く目標値に到達します）
	const float kLerpSpeed = 0.05f;

};