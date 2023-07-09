#include "MultiTexObject.hlsli"

Texture2D<float4> mainTex : register(t0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> subTex : register(t1); // 1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> maskTex : register(t2); // 2�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);

float2 TexTransform(float2 uv, uint texIndex)
{
    return uv * texProps[texIndex].tiling + texProps[texIndex].offset;
}

float4 main(VSOutput input) : SV_TARGET
{
    float4 mainColor = mainTex.Sample(smp, TexTransform(input.uv, 0));
    float4 subColor = subTex.Sample(smp, TexTransform(input.uv, 1));
    float mask = maskTex.Sample(smp, TexTransform(input.uv, 2)).r;
    float4 outputColor = lerp(mainColor, subColor, mask);
    return outputColor;
}