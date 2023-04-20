#include "FbxObject3d.h"
#include "DirectXCommon.h"
#include "D3D12Common.h"
using namespace Microsoft::WRL;
using namespace DirectX;

ComPtr<ID3D12RootSignature> FbxObject3d::rootsignature;
ComPtr<ID3D12PipelineState> FbxObject3d::pipelinestate;

void FbxObject3d::CreateGraphicsPipeline()
{
	PipelineManager pManager;
	// �V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	pManager.LoadShaders(L"FBXVS", L"FBXPS");
	// ���_���C�A�E�g
	pManager.AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	pManager.AddInputLayout("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	pManager.AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	pManager.SetBlendDesc(D3D12_BLEND_OP_ADD, D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA);
	// �f�v�X�X�e���V���X�e�[�g
	pManager.InitDepthStencilState();
	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	pManager.InitDSVFormat();
	// �}�`�̌`��ݒ�i�O�p�`�j
	pManager.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	// ���[�g�p�����[�^
	pManager.AddRootParameter(PipelineManager::RootParamType::CBV); // CBV�i���W�ϊ��s��p�j
	pManager.AddRootParameter(PipelineManager::RootParamType::DescriptorTable); // SRV�i�e�N�X�`���j
	// �O���t�B�b�N�X�p�C�v���C���̐���
	pManager.CreatePipeline(pipelinestate, rootsignature);
}

void FbxObject3d::Initialize(WorldTransform* worldTransform)
{
	HRESULT result;
	// �萔�o�b�t�@�̐���
	result = DirectXCommon::GetInstance()->GetDevice()->
		CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTransform) + 0xff) & ~0xff),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuffTransform));

	this->worldTransform = worldTransform;
	this->worldTransform->Initialize();
}

void FbxObject3d::Update()
{
	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	worldTransform->Update();

	HRESULT result;
	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataTransform* constMap = nullptr;
	result = constBuffTransform->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->viewproj = WorldTransform::GetViewProjection()->GetViewProjectionMatrix();
		constMap->world = model->GetModelTransform() * worldTransform->matWorld;
		constMap->cameraPos = WorldTransform::GetViewProjection()->eye;
		constBuffTransform->Unmap(0, nullptr);
	}
}

void FbxObject3d::Draw()
{
	// ���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
	if (model == nullptr) { return; }

	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipelinestate.Get());
	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(rootsignature.Get());
	// �v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffTransform->GetGPUVirtualAddress());

	// ���f���`��
	model->Draw();
}