cbuffer ConstBufferData : register(b0)
{
	matrix mat;
	float4 color;
}

struct VSOutput
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};