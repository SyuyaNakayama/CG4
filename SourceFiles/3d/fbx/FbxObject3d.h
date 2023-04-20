#pragma once
#include "FbxModel.h"
#include "WorldTransform.h"

class FbxObject3d
{
protected: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public: // �T�u�N���X
	// �萔�o�b�t�@�p�f�[�^�\���́i���W�ϊ��s��p�j
	struct ConstBufferDataTransform
	{
		Matrix4 viewproj;    // �r���[�v���W�F�N�V�����s��
		Matrix4 world; // ���[���h�s��
		Vector3 cameraPos; // �J�������W�i���[���h���W�j
	};

public: // �ÓI�����o�֐�
	/// <summary>
	/// �O���t�B�b�N�p�C�v���C���̐���
	/// </summary>
	static void CreateGraphicsPipeline();

private: // �ÓI�����o�ϐ�
	// �f�o�C�X
	// ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootsignature;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelinestate;

public: // �����o�֐�
	/// <summary>
	/// ������
	/// </summary>
	void Initialize(WorldTransform* worldTransform);

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	void SetModel(FbxModel* model) { this->model = model; }

protected: // �����o�ϐ�
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffTransform;
	WorldTransform* worldTransform;
	// ���f��
	FbxModel* model = nullptr;
};

