#pragma once
#include "FbxModel.h"
#include "WorldTransform.h"
#include "LightGroup.h"

class FbxObject3d
{
protected:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	static const int MAX_BONES = 32;
	
	// �萔�o�b�t�@�p�f�[�^�\���́i���W�ϊ��s��p�j
	struct ConstBufferData
	{
		Matrix4 viewproj;    // �r���[�v���W�F�N�V�����s��
		Matrix4 world; // ���[���h�s��
		Vector3 cameraPos; // �J�������W�i���[���h���W�j
	};

	struct ConstBufferDataSkin
	{
		Matrix4 bones[MAX_BONES];
	};
	
private: // �ÓI�����o�ϐ�
	// ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootsignature;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelinestate;
	static LightGroup* lightGroup;

protected: // �����o�ϐ�
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff, constBuffSkin;
	ConstBufferData* constMap = nullptr;
	ConstBufferDataSkin* constMapSkin = nullptr;
	WorldTransform* worldTransform;
	// ���f��
	FbxModel* model = nullptr;
	FbxTime frameTime, startTime, endTime, currentTime;
	bool isPlay = false;

public:
	static void SetLightGroup(LightGroup* lightGroup) { FbxObject3d::lightGroup = lightGroup; }

	/// <summary>
	/// �O���t�B�b�N�p�C�v���C���̐���
	/// </summary>
	static void CreateGraphicsPipeline();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(WorldTransform* worldTransform, FbxModel* model);

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	void PlayAnimation();
};

