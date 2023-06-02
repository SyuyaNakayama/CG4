#include "FbxObject3d.h"
#include "DirectXCommon.h"
#include "D3D12Common.h"
#include <array>
using namespace Microsoft::WRL;
using namespace DirectX;

ComPtr<ID3D12RootSignature> FbxObject3d::rootsignature;
ComPtr<ID3D12PipelineState> FbxObject3d::pipelinestate;
LightGroup* FbxObject3d::lightGroup = nullptr;

void FbxObject3d::CreateGraphicsPipeline()
{
	PipelineManager pManager;
	// �V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	pManager.LoadShaders(L"FBXVS", L"FBXPS");
	// ���_���C�A�E�g
	pManager.AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	pManager.AddInputLayout("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	pManager.AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	// �e�����󂯂�{�[���ԍ�(4��)
	pManager.AddInputLayout("BONEINDICES", DXGI_FORMAT_R32G32B32A32_UINT);
	// �{�[���̃X�L���E�F�C�g(4��)
	pManager.AddInputLayout("BONEWEIGHTS", DXGI_FORMAT_R32G32B32A32_FLOAT);
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
	
	std::array<CD3DX12_DESCRIPTOR_RANGE, 3> descRangeSRVs{};
	for (UINT i = 0; i < descRangeSRVs.size(); i++) { descRangeSRVs[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i); }

	CD3DX12_ROOT_PARAMETER rootParam{};
	rootParam.InitAsDescriptorTable(descRangeSRVs.size(), descRangeSRVs.data());
	pManager.rootParams.push_back(rootParam);

	pManager.AddRootParameter(PipelineManager::RootParamType::CBV); // CBV�i�X�L�j���O�p�j
	pManager.AddRootParameter(PipelineManager::RootParamType::CBV); // CBV�i�}�e���A���p�j
	pManager.AddRootParameter(PipelineManager::RootParamType::CBV); // CBV�i���C�g�p�j
	// �O���t�B�b�N�X�p�C�v���C���̐���
	pManager.CreatePipeline(pipelinestate, rootsignature);
}

void FbxObject3d::Initialize(WorldTransform* worldTransform, FbxModel* model)
{
	// �萔�o�b�t�@�̐���
	CreateBuffer(&constBuff, &constMap, (sizeof(ConstBufferData) + 0xff) & ~0xff);
	CreateBuffer(&constBuffSkin, &constMapSkin, (sizeof(ConstBufferDataSkin) + 0xff) & ~0xff);

	this->worldTransform = worldTransform;
	this->worldTransform->Initialize();
	this->model = model;

	for (int i = 0; i < MAX_BONES; i++) { constMapSkin->bones[i] = Matrix4::Identity(); }

	// 1�t���[�����̎��Ԃ�60FPS�Őݒ�
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);
	PlayAnimation();
}

void FbxObject3d::Update()
{
	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	worldTransform->Update();

	// �A�j���[�V����
	if (isPlay)
	{
		// 1�t���[���i�߂�
		currentTime += frameTime;
		// �Ō�܂ōĐ�������擪�ɖ߂�
		if (currentTime > endTime) { currentTime = startTime; }
	}

	std::vector<FbxModel::Bone>& bones = model->GetBones();
	for (int i = 0; i < bones.size(); i++)
	{
		FbxAMatrix fbxCurrentPose = bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
		Matrix4 matCurrentPose;
		FbxModel::ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);
		constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPose;
	}

	// �萔�o�b�t�@�փf�[�^�]��
	constMap->viewproj = WorldTransform::GetViewProjection()->GetViewProjectionMatrix();
	constMap->world = model->GetModelTransform() * worldTransform->matWorld;
	constMap->cameraPos = WorldTransform::GetViewProjection()->eye;
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
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootConstantBufferView(2, constBuffSkin->GetGPUVirtualAddress());
	lightGroup->Draw(4);
	// ���f���`��
	model->Draw();
}

void FbxObject3d::PlayAnimation()
{
	FbxScene* fbxScene = model->GetFbxScene();
	// 0�Ԃ̃A�j���[�V�����擾
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	// �A�j���[�V�����̖��O�擾
	const char* animstackname = animstack->GetName();
	// �A�j���[�V�����̎��Ԏ擾
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);
	// �J�n���Ԏ擾
	startTime = takeinfo->mLocalTimeSpan.GetStart();
	// �I�����Ԏ擾
	endTime = takeinfo->mLocalTimeSpan.GetStop();
	// �J�n���Ԃɍ��킹��
	currentTime = startTime;
	// �Đ�����Ԃɂ���
	isPlay = true;
}
