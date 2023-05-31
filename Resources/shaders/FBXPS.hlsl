#include "FBX.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー
static const float PI = 3.141592654f; // π
static float3 N; // 反射点の法線ベクトル

// 双方向反射分布関数
float3 BRDF(float3 L, float3 V)
{
	float NdotL = dot(N, L); // 法線とライト方向の内積
	float NdotV = dot(N, V); // 法線とカメラ方向の内積
	if (NdotL < 0 || NdotV < 0) { return float3(0, 0, 0); } // どちらかが90°以上であれば真っ黒を返す
	float diffuseReflectance = 1.0f / PI; // 拡散反射率
	float3 diffuseColor = diffuseReflectance * NdotL * baseColor * (1 - metalness); // 拡散反射項
	return diffuseColor;
}

float4 main(VSOutput input) : SV_TARGET
{
	N = input.normal;
	float3 finalRGB = float3(0, 0, 0);
	float3 eyedir = normalize(cameraPos - input.worldpos.xyz);
	for (int i = 0; i < DIRLIGHT_NUM; i++)
	{
		if (!dirLights[i].active) { continue; }
		finalRGB += BRDF(dirLights[i].lightv, eyedir) * dirLights[i].lightcolor;
	}

	return float4(finalRGB, 1);
	//// テクスチャマッピング
	//float4 texcolor = tex.sample(smp, input.uv);
	//// lambert反射
	//float3 light = normalize(float3(1,-1,1)); // 右下奥　向きのライト
	//float diffuse = saturate(dot(-light, input.normal));
	//float brightness = diffuse + 0.3f;
	//float4 shadecolor = float4(brightness, brightness, brightness, 1.0f);
	//// 陰影とテクスチャの色を合成
	//return shadecolor * texcolor;
}