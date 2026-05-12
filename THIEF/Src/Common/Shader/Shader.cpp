#include "Shader.h"

#include "../../Application.h"
#include "../../Scene/SceneManager.h"

void Shader::Init(void)
{
	psCtr_ = LoadPixelShader("Src/Common/Shader/CtrShader.pso");
	psCtrConstBuf_ = CreateShaderConstantBuffer(sizeof(Ctr));
	// ポリゴン生成
	MakeSquereVertex();
}

void Shader::Draw(int texture)
{
	// シェーダー設定

	SetUsePixelShader(psCtr_);
	SetUseTextureToShader(0, texture);

	// ピクセルシェーダー用の定数バッファのアドレスを取得
	Ctr* cbBuf = (Ctr*)GetBufferShaderConstantBuffer(psCtrConstBuf_);

	// 走査線の濃さ：0.0(なし) ～ 1.0(真っ黒)
	cbBuf->scanlineIntensity = 0.1f;

	// ビネットの鋭さ：0.0(なし)～1.0（多め）
	// 大きいほど画面の四隅が暗くなる
	cbBuf->vignettePower = 0.5f;

	// グリッチの横ずれ幅：0.0(なし) ～ 1.0(画面半分以上)
	// ずれすぎるので、小さめで設定した方がよい
	cbBuf->glitchAmount = 0.0f;

	// シェーダー内の時間経過：累積時間を秒単位で渡す
	// 個人で設定する必要なし
	cbBuf->timer = SceneManager::GetInstance()->GetTotalTime();

	// 画面の湾曲度：0.0(なし)～1.0（魚眼レンズのように膨らむ）
	// これも膨らみすぎると見えずらいため、小さめで設定した方がよい
	cbBuf->curvatureAmount = 0.01f;

	// 砂嵐ノイズの強さ：0.0(なし) ～ 1.0(砂嵐のみ)
	// 画面が見えずらくなるため、小さめで設定した方がよい
	cbBuf->noisePower = 0.1f;

	// 色ずれ（色収差）の距離：0.0(なし)～1.0(ずれる)
	// 大きすぎると見えずらいため、小さめで設定した方がよい
	cbBuf->rgbShift = 0.002f;

	// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映
	UpdateShaderConstantBuffer(psCtrConstBuf_);

	// ピクセルシェーダー用の定数バッファを定数バッファレジスタにセット
	SetShaderConstantBuffer(psCtrConstBuf_, DX_SHADERTYPE_PIXEL, 1);

	SetTextureAddressModeUV(DX_TEXADDRESS_WRAP, DX_TEXADDRESS_WRAP);

	// 描画
	DrawPolygonIndexed2DToShader(mVertex, 4, mIndex, 2);
}

void Shader::Release(void)
{
	DeleteShader(psCtr_);
	DeleteShaderConstantBuffer(psCtrConstBuf_);
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