#include "FBX.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー
static const float PI = 3.141592654f; // π
static float3 N; // 反射点の法線ベクトル

/*
* Schlickによる近似
* f0 と f90 の値を(1 - cosine)の5乗でlerpする
* f0 : 光が垂直に入射したときの反射率
* f90 : 光が平行に入射したときの反射率
* cosine : 2ベクトルのなす角のコサイン(内積値)
*/
float SchlickFresnel(float f0, float f90, float cosine)
{
	float m = saturate(1 - cosine);
	float m2 = m * m;
	float m5 = m2 * m2 * m;
	return lerp(f0, f90, m5);
}

/*
* UE4のGGX分布
* alpha : roughnessの2乗
* NdotH : 法線とハーフベクトルの内積
*/
float DistributionGGX(float alpha, float NdotH)
{
	float alpha2 = alpha * alpha;
	float t = NdotH * NdotH * (alpha2 - 1.0f) + 1.0f;
	return alpha2 / (PI * t * t);
}

// 鏡面反射の計算
float3 CookTorranceSpecular(float NdotL, float NdotV, float NdotH, float LdotH)
{
	float Ds = DistributionGGX(roughness * roughness, NdotH);				// D項(分布:Distribution)
	float Fs = float3(1, 1, 1);		// F項(フレネル:Fresnel)
	float Gs = 0.1f;				// G項(幾何減衰:Geometry attenuation)
	float m = 4.0f * NdotL * NdotV;	// m項(分母)
	return Ds * Fs * Gs / m;		// 合成して鏡面反射の色を得る
}

// 双方向反射分布関数
float3 BRDF(float3 L, float3 V)
{
	float NdotL = dot(N, L); // 法線とライト方向の内積
	float NdotV = dot(N, V); // 法線とカメラ方向の内積
	if (NdotL < 0 || NdotV < 0) { return float3(0, 0, 0); } // どちらかが90°以上であれば真っ黒を返す

	float3 H = normalize(L + V); // ライト方向とカメラ方向の中間(ハーフベクトル)
	float NdotH = dot(N, H); // 法線とハーフベクトルの内積
	float LdotH = dot(L, H); // ライトとハーフベクトルの内積

	float diffuseReflectance = 1.0f / PI; // 拡散反射率

	float energyBias = 0.5 * roughness;
	float Fd90 = energyBias + 2 * LdotH * LdotH * roughness; // 入射角が90°の場合の拡散反射率
	float FL = SchlickFresnel(1.0f, Fd90, NdotL); // 入っていく時の拡散反射率
	float FV = SchlickFresnel(1.0f, Fd90, NdotV); // 出ていく時の拡散反射率
	float energyFactor = lerp(1.0f, 1.0f / 1.51f, roughness);
	float Fd = FL * FV * energyFactor; // 入って出ていくまでの拡散反射率

	float3 diffuseColor = diffuseReflectance * Fd * baseColor * (1 - metalness); // 拡散反射項
	float3 specularColor = CookTorranceSpecular(NdotL, NdotV, NdotH, LdotH); // 鏡面反射項
	return diffuseColor + specularColor; // 拡散反射と鏡面反射の合計で色が決まる
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