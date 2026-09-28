#include "../../Application.h"
#include "../../Scene/SceneManager.h"

#include "Shader.h"

void Shader::Init(void)
{
	// ピクセルシェーダーの読み込み
	psCtr_ = LoadPixelShader("Data/CtrShader.pso");

	// ピクセルシェーダー用の定数バッファ
	psCtrConstBuf_ = CreateShaderConstantBuffer(sizeof(Ctr));

	// 初期値のセット
	targetCtrParam_ = DEFAULT_CTR;	// 目標値
	currentCtrParam_ = DEFAULT_CTR;	// 現在地

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

	// ピクセルシェーダーをセット
	SetUsePixelShader(psCtr_);

	// テクスチャをセット
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
	DrawPolygonIndexed2DToShader(vertex_, SQUARE_VERTEX_NUM, index_, 2);
}

void Shader::Release(void)
{
	// ピクセルシェーダーの解放
	DeleteShader(psCtr_);

	// ピクセルシェーダー用の定数バッファの解放
	DeleteShaderConstantBuffer(psCtrConstBuf_);
}

void Shader::SetCurvatureAmount(float val, bool isLerpActive)
{
	// 目標値をセット
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
	// 目標値をセット
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
	// 現在値とデフォルト値の差が0.0001未満ならデフォルト値とみなす
	return fabsf(currentCtrParam_.scanlineIntensity - DEFAULT_CTR.scanlineIntensity) < 0.0001f &&
		fabsf(currentCtrParam_.vignettePower - DEFAULT_CTR.vignettePower) < 0.0001f &&
		fabsf(currentCtrParam_.glitchAmount - DEFAULT_CTR.glitchAmount) < 0.0001f &&
		fabsf(currentCtrParam_.curvatureAmount - DEFAULT_CTR.curvatureAmount) < 0.0001f &&
		fabsf(currentCtrParam_.noisePower - DEFAULT_CTR.noisePower) < 0.0001f &&
		fabsf(currentCtrParam_.rgbShift - DEFAULT_CTR.rgbShift) < 0.0001f &&
		fabsf(currentCtrParam_.glitchProbability - DEFAULT_CTR.glitchProbability) < 0.0001f;
}

void Shader::MakeSquereVertex(void)
{
	// ４頂点の初期化
	for (int i = 0; i < SQUARE_VERTEX_NUM; i++)
	{
		vertex_[i].rhw = 1.0f;
		vertex_[i].dif = GetColorU8(255, 255, 255, 255);
		vertex_[i].spc = GetColorU8(255, 255, 255, 255);
		vertex_[i].su = 0.0f;
		vertex_[i].sv = 0.0f;
	}

	// シェーダー追加時の作業を減らすため、あらかじめ四角形の頂点を作成
	int cnt = 0;
	float sX = 0;
	float sY = 0;
	float eX = static_cast<float>(Application::SCREEN_SIZE_X);
	float eY = static_cast<float>(Application::SCREEN_SIZE_Y);

	// 左上
	vertex_[cnt].pos = VGet(sX, sY, 0.0f);
	vertex_[cnt].u = 0.0f;
	vertex_[cnt].v = 0.0f;
	cnt++;

	// 右上
	vertex_[cnt].pos = VGet(eX, sY, 0.0f);
	vertex_[cnt].u = 1.0f;
	vertex_[cnt].v = 0.0f;
	cnt++;

	// 右下
	vertex_[cnt].pos = VGet(eX, eY, 0.0f);
	vertex_[cnt].u = 1.0f;
	vertex_[cnt].v = 1.0f;
	cnt++;

	// 左下
	vertex_[cnt].pos = VGet(sX, eY, 0.0f);
	vertex_[cnt].u = 0.0f;
	vertex_[cnt].v = 1.0f;

	// 頂点インデックス
	cnt = 0;
	index_[cnt++] = 0;
	index_[cnt++] = 1;
	index_[cnt++] = 3;

	index_[cnt++] = 1;
	index_[cnt++] = 2;
	index_[cnt++] = 3;
}

void Shader::UpdateParam(float& current, float target)
{
	// 目標値と現在地の差が0.0001未満なら、現在地を目標値に合わせる
	if (fabsf(target - current) < 0.0001f)
	{
		// 目標値と現在地を同期させる
		current = target;
	}
	else
	{
		// 線形補間で現在地を目標値に近づける
		current += (target - current) * LERP_SPEED;
	}
}
