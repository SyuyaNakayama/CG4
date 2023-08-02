cbuffer cbuff0 : register(b0)
{
	matrix world; // ワールド行列
};

cbuffer cbuff1 : register(b1)
{
	float3 m_ambient  : packoffset(c0); // アンビエント係数
	float3 m_diffuse  : packoffset(c1); // ディフューズ係数
	float3 m_specular : packoffset(c2); // スペキュラー係数
	float m_alpha : packoffset(c2.w);	// アルファ
}

cbuffer cbuff2 : register(b2)
{
	matrix viewproj; // ビュープロジェクション行列
	float3 cameraPos; // カメラ座標(ワールド座標)
};

struct TexProp
{
    float2 tiling;
    float2 offset;
};

cbuffer cbuff3 : register(b3)
{
    TexProp texProps[3];
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos : SV_POSITION; // システム用頂点座標
	float4 worldpos : POSITION; // ワールド座標
	float3 normal : NORMAL; // 法線
	float2 uv : TEXCOORD; // uv値
	float4 spriteColor : COLOR; // スプライト色
};