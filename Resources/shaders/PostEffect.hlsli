cbuffer cbuff0 : register(b0)
{
	uint effectType;
    float angle; // ‚Ú‚©‚·Šp“x(ƒ‰ƒWƒAƒ“)
}

struct VSOutput
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};