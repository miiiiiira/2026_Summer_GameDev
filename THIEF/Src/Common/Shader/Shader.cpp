#include "Shader.h"

#include "../../Application.h"
#include "../../Scene/SceneManager.h"

void Shader::Init(void)
{
	psCtr_ = LoadPixelShader("Data/CtrShader.pso");
	psCtrConstBuf_ = CreateShaderConstantBuffer(sizeof(Ctr));

	// 初期値のセット
	targetCtrParam_ = DEFAULT_CTR;
	currentCtrParam_ = DEFAULT_CTR;

	// ポリゴン生成
	MakeSquereVertex();
}

void Shader::Draw(int texture)
{
	// スキャンライン
	UpdateParam(currentCtrParam_.scanlineIntensity, targetCtrParam_.scanlineIntensity);
	// ビネット
	UpdateParam(currentCtrParam_.vignettePower, targetCtrParam_.vignettePower);
	// グリッチ
	UpdateParam(currentCtrParam_.glitchAmount, targetCtrParam_.glitchAmount);
	// 魚眼
	UpdateParam(currentCtrParam_.curvatureAmount, targetCtrParam_.curvatureAmount);
	// ノイズ
	UpdateParam(currentCtrParam_.noisePower, targetCtrParam_.noisePower);
	// RGBずらし
	UpdateParam(currentCtrParam_.rgbShift, targetCtrParam_.rgbShift);

	// タイマーは毎フレーム更新
	currentCtrParam_.timer = SceneManager::GetInstance()->GetTotalTime();

	// シェーダー設定
	SetUsePixelShader(psCtr_);
	SetUseTextureToShader(0, texture);

	// ピクセルシェーダー用の定数バッファのアドレスを取得
	Ctr* cbBuf = (Ctr*)GetBufferShaderConstantBuffer(psCtrConstBuf_);
	*cbBuf = currentCtrParam_;

	// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映
	UpdateShaderConstantBuffer(psCtrConstBuf_);

	// ピクセルシェーダー用の定数バッファを定数バッファレジスタにセット
	SetShaderConstantBuffer(psCtrConstBuf_, DX_SHADERTYPE_PIXEL, 1);

	// 画面外を黒枠にする
	SetTextureAddressModeUV(DX_TEXADDRESS_BORDER, DX_TEXADDRESS_BORDER);

	// 描画
	DrawPolygonIndexed2DToShader(mVertex, 4, mIndex, 2);
}

void Shader::Release(void)
{
	DeleteShader(psCtr_);
	DeleteShaderConstantBuffer(psCtrConstBuf_);
}

void Shader::SetCurvatureAmount(float val, bool isLerpActive)
{
	targetCtrParam_.curvatureAmount = val;

	// 補間しないなら
	if (!isLerpActive)
	{
		// 強制的に値を同期させる
		currentCtrParam_.curvatureAmount = val;
	}
}

void Shader::SetGlitchProbability(float val, bool isLerpActive)
{
	targetCtrParam_.glitchProbability = val;

	// 補間しないなら
	if (!isLerpActive)
	{
		// 強制的に値を同期させる
		currentCtrParam_.glitchProbability = val;
	}
}

bool Shader::IsDefault(void) const
{
	return fabsf(currentCtrParam_.scanlineIntensity - DEFAULT_CTR.scanlineIntensity) < 0.0001f &&
		fabsf(currentCtrParam_.vignettePower - DEFAULT_CTR.vignettePower) < 0.0001f &&
		fabsf(currentCtrParam_.glitchAmount - DEFAULT_CTR.glitchAmount) < 0.0001f &&
		fabsf(currentCtrParam_.curvatureAmount - DEFAULT_CTR.curvatureAmount) < 0.0001f &&
		fabsf(currentCtrParam_.noisePower - DEFAULT_CTR.noisePower) < 0.0001f &&
		fabsf(currentCtrParam_.rgbShift - DEFAULT_CTR.rgbShift) < 0.0001f;
}

void Shader::MakeSquereVertex(void)
{

	// 毎回頂点データを作成するのは無駄ですが、
	// シェーダー追加時の作業を減らすため、毎フレーム作成

	int cnt = 0;
	float sX = 0;
	float sY = 0;
	float eX = static_cast<float>(Application::SCREEN_SIZE_X);
	float eY = static_cast<float>(Application::SCREEN_SIZE_Y);

	// ４頂点の初期化
	for (int i = 0; i < 4; i++)
	{
		mVertex[i].rhw = 1.0f;
		mVertex[i].dif = GetColorU8(255, 255, 255, 255);
		mVertex[i].spc = GetColorU8(255, 255, 255, 255);
		mVertex[i].su = 0.0f;
		mVertex[i].sv = 0.0f;
	}

	// 左上
	mVertex[cnt].pos = VGet(sX, sY, 0.0f);
	mVertex[cnt].u = 0.0f;
	mVertex[cnt].v = 0.0f;
	cnt++;

	// 右上
	mVertex[cnt].pos = VGet(eX, sY, 0.0f);
	mVertex[cnt].u = 1.0f;
	mVertex[cnt].v = 0.0f;
	cnt++;

	// 右下
	mVertex[cnt].pos = VGet(eX, eY, 0.0f);
	mVertex[cnt].u = 1.0f;
	mVertex[cnt].v = 1.0f;
	cnt++;

	// 左下
	mVertex[cnt].pos = VGet(sX, eY, 0.0f);
	mVertex[cnt].u = 0.0f;
	mVertex[cnt].v = 1.0f;
	// 頂点インデックス
	cnt = 0;
	mIndex[cnt++] = 0;
	mIndex[cnt++] = 1;
	mIndex[cnt++] = 3;

	mIndex[cnt++] = 1;
	mIndex[cnt++] = 2;
	mIndex[cnt++] = 3;
}

void Shader::UpdateParam(float& current, float target)
{
	if (fabsf(target - current) < 0.0001f)
	{
		current = target;
	}
	else
	{
		current += (target - current) * kLerpSpeed;
	}
}
