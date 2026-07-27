// ピクセルシェーダーの入力
struct PS_INPUT
{
    float4 Position : SV_POSITION;
	// 座標( プロジェクション空間 )
    float4 Diffuse : COLOR0;
	// ディフューズカラー
    float2 TexCoords0 : TEXCOORD0;
	// テクスチャ座標
};

// ノイズ生成用
static const float2 NOISE_SEED = float2(12.9898f, 78.233f);
static const float NOISE_AMPLITUDE = 43758.545f;

// グリッチの発生間隔
static const float TIME_RANDOM_SEED_A = 123.456f;
static const float TIME_RANDOM_SEED_B = 789.012f;

// グリッチ演出用
static const float GLITCH_CHECK_HERTZ = 2.0f;       // 1秒間に行うグリッチ判定
static const float GLITCH_PROBABILITY = 0.8f;       // グリッチが発生しない率
static const float GLITCH_WAVE_FREQ = 40.0f;       // グリッチ線の細かさ
static const float GLITCH_WAVE_SPEED = 0.5f;        // グリッチの波が上下に流れる速度 
static const float GLITCH_THRESHOLD = 0.95f;         // どの程度の波の強さでグリッチを有効にするか

// 走査線の密度
static const float SCANLINE_DENSITY = 500.0f;

cbuffer cbParam : register(b1)
{
    float g_scanlineIntensity;      // 走査線の濃さ
    float g_vignettePower;          // ビネットの鋭さ
    float g_glitchAmount;           // グリッチの強度　0.0で通常、数字が大きくなると大きく揺れる
    float g_timer;                  // ノイズやグリッチを動かす時間
    float g_curvatureAmount;        // 曲面の歪み度
    float g_noisePower;             // ノイズの強度
    float g_rgbShift;               // 色のずれ
    float glitchProbability;
}

// 描画するテクスチャ
Texture2D g_SrcTexture : register(t0);

// サンプラー：適切な色を決める処理
SamplerState g_SrcSampler : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.TexCoords0;
   
    // 中心を0.0にする
    float2 center = (uv - 0.5f);
   
    // 中心からの距離を計算
    // * 2.0で0.0～1.0にする
    float x = abs(center.x) * 2.0f;
    float y = abs(center.y) * 2.0f;
   
    // 魚眼レンズ処理
    // --------------------------------------------
    // 歪みの計算
    if (g_curvatureAmount > 0)
    {
        // 中心から離れるほど、歪みが強くなる
        float distortion = (x * x + y * y) * g_curvatureAmount;
    
        // 曲面の歪み
        uv = 0.5f + center * (1.0f + distortion);
    }
    
    // グリッチ処理
    // --------------------------------------------
    if (g_glitchAmount > 0)
    {
       // 0.5秒ごとに値を更新
        float timeStep = floor(g_timer * GLITCH_CHECK_HERTZ);
        float randomInterval = frac(sin(timeStep * TIME_RANDOM_SEED_A) * TIME_RANDOM_SEED_B);
    
        if (randomInterval > glitchProbability)
        {
             // グリッチ
            float glitch = sin(uv.y * GLITCH_WAVE_FREQ + g_timer * GLITCH_WAVE_SPEED);
    
            if (abs(glitch) > GLITCH_THRESHOLD)
            {
                uv.x += glitch * g_glitchAmount;
            }
        }
    }
    
    float4 color = g_SrcTexture.Sample(g_SrcSampler, uv);

    // RGBずらし処理
    // --------------------------------------------
    if (g_rgbShift > 0)
    {
       // RGBずらし用の座標
        float2 uvRed = uv + float2(g_rgbShift, 0.0f);
        float2 uvBlue = uv - float2(g_rgbShift, 0.0f);
        
        // 左方向にずらす
        float r = g_SrcTexture.Sample(g_SrcSampler, uvRed).r;
        // そのまま
        float g = g_SrcTexture.Sample(g_SrcSampler, uv).g;
        // 右方向にずらす
        float b = g_SrcTexture.Sample(g_SrcSampler, uvBlue).b;
        
        color = float4(r, g, b, 1.0f);
    }
        
        // ビネット
    // --------------------------------------------
    if (g_vignettePower > 0)
    {
        float vignette = (1.0f - x * x * g_vignettePower)
                              * (1.0f - y * y * g_vignettePower);
        color.rgb *= vignette;
    }
    
    // ノイズ処理
    // --------------------------------------------
    // ノイズ計算
    if (g_noisePower > 0)
    {
        float noise = frac(sin(
			dot(uv * frac(g_timer), NOISE_SEED)) * NOISE_AMPLITUDE) - 0.5f;
    
        // ノイズカラーを加算する
        color.rgb += noise * g_noisePower;
    }

    
    // スキャンライン処理
    // --------------------------------------------
    if (g_scanlineIntensity > 0)
    {
        float scanLine = 1.0f - abs(sin(uv.y * SCANLINE_DENSITY)) * g_scanlineIntensity;
        color.rgb *= scanLine;
    }

    return color;
}