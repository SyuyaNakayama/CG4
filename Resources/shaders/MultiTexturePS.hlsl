#include "PostEffectFunctions.hlsli"

Texture2D<float4> tex0 : register(t0); // 0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1); // 1番スロットに設定されたテクスチャ

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor0 = tex0.Sample(smp, input.uv);
    float4 texcolor1 = tex1.Sample(smp, input.uv);
    float4 outputColor = texcolor0 + texcolor1;
    return outputColor;
}