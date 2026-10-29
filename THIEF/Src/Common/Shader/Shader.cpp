#include "../../Application.h"
#include "../../Scene/SceneManager.h"

#include "Shader.h"

namespace
{
	// パラメータが目標値に到達したとみなす誤差
	constexpr float PARAM_EPSILON = 0.0001f;

	// 描画元テクスチャのスロット番号
	constexpr int SRC_TEXTURE_SLOT = 0;

	// 定数バッファのスロット番号
	constexpr int CRT_CBUFFER_SLOT = 1;

	// 四角形を構成する三角形ポリゴンの数
	constexpr int SQUARE_POLYGON_NUM = 2;
}

Shader::Shader(void)
{
}

Shader::~Shader(void)
{
}

void Shader::Init(void)
{
	// ピクセルシェーダーの読み込み
	psCrt_ = LoadPixelShader("Data/CRTShader.pso");

	// ピクセルシェーダー用の定数バッファ
	psCrtConstBuf_ = CreateShaderConstantBuffer(sizeof(CrtParam));

	// 初期値のセット
	targetCrtParam_ = DEFAULT_CRT;	// 目標値
	currentCrtParam_ = DEFAULT_CRT;	// 現在値

	// ポリゴン生成
	MakeSquareVertex();
}

void Shader::Draw(int texture)
{
	// スキャンライン
	UpdateParam(currentCrtParam_.scanlineIntensity, targetCrtParam_.scanlineIntensity);

	// ビネット
	UpdateParam(currentCrtParam_.vignettePower, targetCrtParam_.vignettePower);

	// グリッチ
	UpdateParam(currentCrtParam_.glitchAmount, targetCrtParam_.glitchAmount);

	// 魚眼
	UpdateParam(currentCrtParam_.curvatureAmount, targetCrtParam_.curvatureAmount);

	// ノイズ
	UpdateParam(currentCrtParam_.noisePower, targetCrtParam_.noisePower);

	// RGBずらし
	UpdateParam(currentCrtParam_.rgbShift, targetCrtParam_.rgbShift);

	// タイマーは毎フレーム更新
	currentCrtParam_.timer = SceneManager::GetInstance()->GetTotalTime();

	// ピクセルシェーダーをセット
	SetUsePixelShader(psCrt_);

	// テクスチャをセット
	SetUseTextureToShader(SRC_TEXTURE_SLOT, texture);

	// ピクセルシェーダー用の定数バッファのアドレスを取得
	CrtParam* cbBuf = (CrtParam*)GetBufferShaderConstantBuffer(psCrtConstBuf_);
	*cbBuf = currentCrtParam_;

	// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映
	UpdateShaderConstantBuffer(psCrtConstBuf_);

	// ピクセルシェーダー用の定数バッファを定数バッファレジスタにセット
	SetShaderConstantBuffer(psCrtConstBuf_, DX_SHADERTYPE_PIXEL, CRT_CBUFFER_SLOT);

	// 画面外を黒枠にする
	SetTextureAddressModeUV(DX_TEXADDRESS_BORDER, DX_TEXADDRESS_BORDER);

	// 描画
	DrawPolygonIndexed2DToShader(vertex_, SQUARE_VERTEX_NUM, index_, SQUARE_POLYGON_NUM);
}

void Shader::Release(void)
{
	// ピクセルシェーダーの解放
	DeleteShader(psCrt_);

	// ピクセルシェーダー用の定数バッファの解放
	DeleteShaderConstantBuffer(psCrtConstBuf_);
}

void Shader::SetCurvatureAmount(float val, bool isLerpActive)
{
	// 目標値をセット
	targetCrtParam_.curvatureAmount = val;

	// 補間しないなら
	if (!isLerpActive)
	{
		// 強制的に値を同期させる
		currentCrtParam_.curvatureAmount = val;
	}
}

bool Shader::IsDefault(void) const
{
	// 現在値とデフォルト値の差が0.0001未満ならデフォルト値とみなす
	return fabsf(currentCrtParam_.scanlineIntensity - DEFAULT_CRT.scanlineIntensity) < PARAM_EPSILON &&
		fabsf(currentCrtParam_.vignettePower - DEFAULT_CRT.vignettePower) < PARAM_EPSILON &&
		fabsf(currentCrtParam_.glitchAmount - DEFAULT_CRT.glitchAmount) < PARAM_EPSILON &&
		fabsf(currentCrtParam_.curvatureAmount - DEFAULT_CRT.curvatureAmount) < PARAM_EPSILON &&
		fabsf(currentCrtParam_.noisePower - DEFAULT_CRT.noisePower) < PARAM_EPSILON &&
		fabsf(currentCrtParam_.rgbShift - DEFAULT_CRT.rgbShift) < PARAM_EPSILON;
}

void Shader::MakeSquareVertex(void)
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
	// 目標値と現在値の差が0.0001未満なら、現在値を目標値に合わせる
	if (fabsf(target - current) < PARAM_EPSILON)
	{
		// 目標値と現在値を同期させる
		current = target;
	}
	else
	{
		// 線形補間で現在値を目標値に近づける
		current += (target - current) * LERP_SPEED;
	}
}
