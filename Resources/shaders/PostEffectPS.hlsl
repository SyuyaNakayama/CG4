#include "PostEffectFunctions.hlsli"
Texture2D<float4> tex : register(t0);

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    if (effectType == 1)
    {
        texcolor = GetHighLumi(texcolor);
    }
    if (effectType == 2)
    {
        texcolor = GaussianBlur(tex, input);
    }
    if (effectType == 3)
    {
        texcolor = GaussianBlurLinear(tex, input);
    }
    return texcolor;
}