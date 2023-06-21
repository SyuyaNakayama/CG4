#include "PostEffect.hlsli"

Texture2D<float4> tex0 : register(t0); // 0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1); // 1番スロットに設定されたテクスチャ
SamplerState smp : register(s0);

float4 Blur(VSOutput i, float blurRange, Texture2D<float4> tex)
{
    float4 col = float4(0, 0, 0, 0);
    float num = 0;

    for (float py = -blurRange; py <= blurRange; py++)
    {
        for (float px = -blurRange; px <= blurRange; px++)
        {
            float2 pickUV = i.uv + float2(px / 1920.0f, py / 1080.0f);
            col += tex.Sample(smp, pickUV);
            num++;
        }
    }
    col.rgb /= num; // かけた「重み」分、結果から割る
    col.a = 1;
    return col;
}

float4 main(VSOutput input) : SV_TARGET
{
    float4 colortex0 = tex0.Sample(smp, input.uv);
    float4 colortex1 = tex1.Sample(smp, input.uv);

    float4 color = float4(1, 1, 1, 1) - colortex0;
    if (fmod(input.uv.y, 0.1f) < 0.05f)
    {
        color = Blur(input, 3, tex1);
    }

    return float4(color.rgb, 1);
}