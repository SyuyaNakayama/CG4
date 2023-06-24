cbuffer cbuff0 : register(b0)
{
	uint effectType;
    float angle; // ぼかす角度(ラジアン)
}

struct VSOutput
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};