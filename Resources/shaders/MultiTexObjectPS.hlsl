#include "MultiTexObject.hlsli"

Texture2D<float4> mainTex : register(t0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> subTex : register(t1); // 1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> maskTex : register(t2); // 1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    float4 mainColor = mainTex.Sample(smp, input.uv);
    float4 subColor = subTex.Sample(smp, input.uv);
    float mask = maskTex.Sample(smp, input.uv).r;
    float4 outputColor = lerp(mainColor, subColor, mask);
    return outputColor;
}