cbuffer cbuff0 : register(b0)
{
	uint effectType;
    uint index; // �}���`�e�N�X�`���Ń|�X�g�G�t�F�N�g��������e�N�X�`���̃C���f�b�N�X
}

struct VSOutput
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};