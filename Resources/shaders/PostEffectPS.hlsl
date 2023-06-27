#include "PostEffectFunctions.hlsli"
Texture2D<float4> tex : register(t0);

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    
    switch (effectType)
    {
        case 1:
            texcolor = GetHighLumi(texcolor);
            break;
        case 2:
            texcolor = GaussianBlur(tex, input);
            break;
        case 3:
            texcolor = GaussianBlurLinear(tex, input);
            break;
        case 4:
            texcolor = GetHighLumi(texcolor);
            texcolor *= CreateDotFilter(tex, input);
            break;
    }
    return texcolor;
}