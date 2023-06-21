#include "PostEffectFunctions.hlsli"

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    if (input.svpos.w <= 0.0)
    {
        return float4(1, 0, 0, 1);
    }
    return texcolor;
}