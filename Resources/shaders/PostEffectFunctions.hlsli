#include "PostEffect.hlsli"
SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);

float4 ChangeBrightness(float4 col, float brightness)
{
	return float4(col.rgb * brightness, 1);
}

float4 ColorFlip(float4 col)
{
	return float4(float3(1, 1, 1) - col.rgb, 1);
}

float4 ChangeUV(VSOutput i, float2 uvOffset)
{
	return tex.Sample(smp, i.uv + uvOffset);
}

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
	float d = distance(drawUV, pickUV);
	return exp(-(d * d) / (2 * sigma * sigma));
}

float GrayScale(float4 col) { return col.r * 0.299 + col.g * 0.587 + col.b * 0.114; }

float4 GetHighLumi(VSOutput i)
{
	float4 col = tex.Sample(smp, i.uv);
	float grayScale = GrayScale(col);
	float extract = smoothstep(0.6, 0.9, grayScale);
	return col * extract;
}

float4 Blur(VSOutput i, float blurRange)
{
	float4 col = float4(0, 0, 0, 0);
	float num = 0;

	for (float py = -blurRange; py <= blurRange; py++)
	{
		for (float px = -blurRange; px <= blurRange; px++)
		{
			float2 pickUV = i.uv + float2(px / 1920.0f, py / 1080.0f);
			col += tex.Sample(smp, pickUV);
			num++;
		}
	}
	col.rgb /= num; // かけた「重み」分、結果から割る
	col.a = 1;
	return col;
}

float4 GaussianBlur(VSOutput i)
{
	float totalWeight = 0, sigma = 0.005, stepWidth = 0.001;
	float4 col = float4(0, 0, 0, 0);

	for (float py = -sigma * 2; py <= sigma * 2; py += stepWidth)
	{
		for (float px = -sigma * 2; px <= sigma * 2; px += stepWidth)
		{
			float2 pickUV = i.uv + float2(px, py);
			float weight = Gaussian(i.uv, pickUV, sigma);
			// Gaussianで取得した「重み」を色にかける
			col += tex.Sample(smp, pickUV) * weight;
			// かけた「重み」の合計値を控えておく
			totalWeight += weight;
		}
	}
	col.rgb = col.rgb / totalWeight; // かけた「重み」分、結果から割る
	col.a = 1;
	return col;
}

float Bloom(VSOutput i)
{
	float4 highLumi = GetHighLumi(i); // 高輝度抽出
	float4 blur = GaussianBlur(i); // ぼかし
	return tex.Sample(smp, i.uv) + highLumi + blur; // テクスチャ合成
}