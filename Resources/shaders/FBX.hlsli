#include "Light.hlsli"

cbuffer cbuff0 : register(b0)
{
	matrix viewproj; // ビュープロジェクション行列
	matrix world; // ワールド行列
	float3 cameraPos; // カメラ座標（ワールド座標）
};

static const int MAX_BONES = 32;

cbuffer skinning : register(b1)
{
	matrix matSkinning[MAX_BONES];
}

cbuffer cbuff2 : register(b2)
{
	float3 baseColor;
	float metalness;
	float specular;
	float roughness;
}

cbuffer cbuff3 : register(b3)
{
	float3 ambientColor;
	DirLight dirLights[DIRLIGHT_NUM];
	PointLight pointLights[POINTLIGHT_NUM];
	SpotLight spotLights[SPOTLIGHT_NUM];
}

//バーテックスバッファーの入力
struct VSInput
{
	float4 pos	: POSITION; // 位置   
	float3 normal : NORMAL; // 頂点法線
	float2 uv	: TEXCOORD; // テクスチャー座標
	uint4 boneIndices : BONEINDICES;
	float4 boneWeights : BONEWEIGHTS;
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos : SV_POSITION; // システム用頂点座標
	float3 worldpos : POS; // ワールド座標
	float3 normal : NORMAL; // 法線
	float2 uv : TEXCOORD; // uv値
};