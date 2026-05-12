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

	void Init(void);
	void Draw(int texture);
	void Release(void);

private:

	// 頂点情報
	VERTEX2DSHADER mVertex[4];
	WORD mIndex[6];
	int psCtr_;
	int psCtrConstBuf_;


	// 描画用の四角頂点を作成
	void MakeSquereVertex(void);

};

