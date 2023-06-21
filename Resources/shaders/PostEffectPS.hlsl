#include "PostEffectFunctions.hlsli"

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = tex.Sample(smp,input.uv);
	texcolor = GetHighLumi(input);
	return texcolor;
}