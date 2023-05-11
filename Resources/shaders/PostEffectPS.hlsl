#include "PostEffect.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 ToonShading(float4 color)
{
	return smoothstep(0.48f, 0.52f, color);
}

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = tex.Sample(smp,input.uv);
	//texcolor = ToonShading(texcolor);
	return float4(texcolor.rgb,1);
}