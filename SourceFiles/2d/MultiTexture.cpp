#include "MultiTexture.h"
#include "D3D12Common.h"

#pragma region �����֐�
void MultiTexture::CreateGraphicsPipelineState()
{
	PipelineManager pipelineManager;
	pipelineManager.LoadShaders(L"PostEffectVS", L"MultiTexturePS");
	pipelineManager.AddInputLayout("POSITION", DXGI_FORMAT_R32G32_FLOAT);
	pipelineManager.AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	pipelineManager.SetBlendDesc(D3D12_BLEND_OP_ADD, D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA);
	pipelineManager.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pipelineManager.AddRootParameter(PipelineManager::RootParamType::CBV);
	pipelineManager.AddRootParameter(PipelineManager::RootParamType::DescriptorTable);
	pipelineManager.AddRootParameter(PipelineManager::RootParamType::DescriptorTable, 1);
	pipelineManager.CreatePipeline(pipelineState, rootSignature);
}

void MultiTexture::CreateBuffers()
{
	std::array<Vertex, 4> vertices =
	{ {
		{{-1,-1},{0,1}},
		{{-1,+1},{0,0}},
		{{+1,-1},{1,1}},
		{{+1,+1},{1,0}}
	} };

	Vertex* vertMap = nullptr;
	ID3D12Resource* vertBuff = nullptr;
	CreateBuffer<Vertex>(&vertBuff, &vertMap, sizeof(Vertex) * vertices.size());

	// �S���_�ɑ΂��č��W���R�s�[
	copy(vertices.begin(), vertices.end(), vertMap);

	// GPU���z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// ���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeof(Vertex) * (UINT)vertices.size();
	// ���_1���̃f�[�^�T�C�Y
	vbView.StrideInBytes = sizeof(Vertex);
}
#pragma endregion

void MultiTexture::Initialize()
{
	CreateBuffers();
	CreateGraphicsPipelineState();
}

void MultiTexture::Draw(std::array<PostEffect, 2>& postEffects)
{
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();

	// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	cmdList->SetPipelineState(pipelineState.Get());
	cmdList->SetGraphicsRootSignature(rootSignature.Get());
	// �v���~�e�B�u�`��̐ݒ�R�}���h
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // �O�p�`���X�g
	// �f�X�N���v�^�q�[�v�̐ݒ�R�}���h
	ID3D12DescriptorHeap* ppHeaps[] = { PostEffect::GetSRV() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	cmdList->SetGraphicsRootDescriptorTable(1, postEffects[0].GetGPUHandle());

	cmdList->SetGraphicsRootDescriptorTable(2, postEffects[1].GetGPUHandle());

	// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	// �`��R�}���h
	cmdList->DrawInstanced((UINT)vertices.size(), 1, 0, 0); // �S�Ă̒��_���g���ĕ`��
}