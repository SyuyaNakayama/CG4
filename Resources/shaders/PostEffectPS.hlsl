#include "PostEffect.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float A(float color, float mul);

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = tex.Sample(smp,input.uv);
	//texcolor.r = A(texcolor.r,2.0f);
	//texcolor.g = A(texcolor.g,2.0f);
	//texcolor.b = A(texcolor.b,2.0f);
	//texcolor.r *= 1.1f;
	//float len = length(input.uv - float2(0.5f, 0.5f));
	//if (len < 0.25f) { texcolor = tex.Sample(smp, input.uv * 0.5f); }
	//texcolor.r -= sin(texcolor.r*2.0f);
	//texcolor.g -= sin(texcolor.g*2.0f);
	//texcolor.b -= sin(texcolor.b*2.0f);
	//if (input.uv.x > 0.1f && input.uv.x < 0.15f) { texcolor = float4(1,1,1,1)-tex.Sample(smp, input.uv + float2(0,0.03f)); }
	//if (input.uv.x > 0.3f && input.uv.x < 0.35f) { texcolor = float4(1,1,1,1)-tex.Sample(smp, input.uv - float2(0,0.03f)); }
	//if (input.uv.x > 0.5f && input.uv.x < 0.55f) { texcolor = float4(1,1,1,1)-tex.Sample(smp, input.uv + float2(0,0.03f)); }
	//if (input.uv.x > 0.7f && input.uv.x < 0.75f) { texcolor = float4(1,1,1,1)-tex.Sample(smp, input.uv - float2(0,0.03f)); }
	//if (input.uv.x > 0.9f && input.uv.x < 0.95f) { texcolor = float4(1,1,1,1)-tex.Sample(smp, input.uv + float2(0,0.03f)); }
	//if (input.uv.y > 0.1f && input.uv.y < 0.15f) { texcolor = float4(1,1,1,1)-tex.Sample(smp, input.uv + float2(0.03f,0)); }
	//if (input.uv.y > 0.3f && input.uv.y < 0.35f) { texcolor = float4(1,1,1,1)-tex.Sample(smp, input.uv - float2(0.03f,0)); }
	//if (input.uv.y > 0.5f && input.uv.y < 0.55f) { texcolor = float4(1,1,1,1)-tex.Sample(smp, input.uv + float2(0.03f,0)); }
	//if (input.uv.y > 0.7f && input.uv.y < 0.75f) { texcolor = float4(1,1,1,1)-tex.Sample(smp, input.uv - float2(0.03f,0)); }
	//if (input.uv.y > 0.9f && input.uv.y < 0.95f) { texcolor = float4(1,1,1,1)-tex.Sample(smp, input.uv + float2(0.03f,0)); }
	
	return float4(texcolor.rgb,1);
}

float A(float color, float mul)
{
	if (color >= 0.5f) { return color * mul; }
	return color;
}