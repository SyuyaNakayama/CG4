#include "OBJShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float2 randomVec(float2 fact)
{
    float2 angle = float2(
        dot(fact, float2(127.1, 311.7)),
        dot(fact, float2(269.5, 183.3))
    );
    return frac(sin(angle) * 43758.5453123) * 2 - 1;
}

// ノイズの密度をdensityで設定、uvにi.uvを代入
float PerlinNoise(float density, float2 uv)
{
    float2 uvFloor = floor(uv * density);
    float2 uvFrac = frac(uv * density);
    float v[2][2], c[2][2];

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++)
        {
            // 各頂点のランダムなベクトルを取得
            v[i][j] = randomVec(uvFloor + float2(i, j));
            // ランダムなベクトルと頂点に向かうベクトルの内積をとって、各頂点の色を作成
            c[i][j] = dot(v[i][j], uvFrac - float2(i, j));
        }
    }

    float2 u = uvFrac * uvFrac * (3 - 2 * uvFrac);
    // 描画するピクセルからピクセル色を求める
    float v0010 = lerp(c[0][0], c[1][0], u.x);
    float v0111 = lerp(c[0][1], c[1][1], u.x);
    return lerp(v0010, v0111, u.y) / 2 + 0.5;
}

float FractalSumNoise(float density, float2 uv)
{
    float fn;
    for (float i = 1; i < 16; i *= 2)
    {
        fn += PerlinNoise(density * i, uv) * 1.0 / (i * 2);
    }
    return fn;
}

float4 main(VSOutput input) : SV_TARGET
{
    float pn = FractalSumNoise(10, input.uv);
    clip(pn - dissolve);
    return float4(pn, pn, pn, 1);
}