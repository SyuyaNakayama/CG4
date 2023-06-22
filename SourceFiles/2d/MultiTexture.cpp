#include "MultiTexture.h"
#include "D3D12Common.h"
#include "WindowsAPI.h"

const float MultiTexture::CLEAR_COLOR[4] = { 0.1f,0.25f,0.5f,0 };

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

	// �萔�o�b�t�@
	ConstBufferData* constMap = nullptr;
	CreateBuffer(constBuff.GetAddressOf(),
		&constMap, (sizeof(ConstBufferData) + 0xff) & ~0xff);

	constMap->mat = Matrix4::Identity();
	constMap->color = { 1,1,1,1 };

	Result result;
	const Vector2 WIN_SIZE = WindowsAPI::WIN_SIZE;
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, (UINT64)WIN_SIZE.x, (UINT)WIN_SIZE.y,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	for (size_t i = 0; i < 2; i++)
	{
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, CLEAR_COLOR),
			IID_PPV_ARGS(&texBuff[i]));

		// ��f��
		const UINT PIXEL_COUNT = (UINT)WIN_SIZE.x * (UINT)WIN_SIZE.y;
		// �摜1�s���̃f�[�^�T�C�Y
		const UINT ROW_PITCH = sizeof(UINT) * (UINT)WIN_SIZE.x;
		// �摜�S�̂̃f�[�^�T�C�Y
		const UINT DEPTH_PITCH = ROW_PITCH * (UINT)WIN_SIZE.y;
		// �摜�C���[�W
		UINT* img = new UINT[PIXEL_COUNT];
		for (size_t j = 0; j < PIXEL_COUNT; j++) { img[j] = 0xff0000ff; }

		result = texBuff[i]->WriteToSubresource(0, nullptr, img, ROW_PITCH, DEPTH_PITCH);
		delete[] img;
	}
}

void MultiTexture::CreateSRV()
{
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = 2;
	Result result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&descHeapSRV));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	for (size_t i = 0; i < 2; i++)
	{
		device->CreateShaderResourceView(texBuff[i].Get(), &srvDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapSRV->GetCPUDescriptorHandleForHeapStart(), i,
				device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
	}
}

void MultiTexture::CreateRTV()
{
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = 2;
	Result result = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&descHeapRTV));

	for (size_t i = 0; i < 2; i++)
	{
		device->CreateRenderTargetView(texBuff[i].Get(), nullptr,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapRTV->GetCPUDescriptorHandleForHeapStart(), i,
				device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)));
	}
}

void MultiTexture::CreateDSV()
{
	const Vector2 WIN_SIZE = WindowsAPI::WIN_SIZE;
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	CD3DX12_RESOURCE_DESC depthResourceDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			(UINT64)WIN_SIZE.x, (UINT)WIN_SIZE.y,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	Result result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE, &depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuff));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors = 1;
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&descHeapDSV));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(depthBuff.Get(), &dsvDesc, descHeapDSV->GetCPUDescriptorHandleForHeapStart());
}
#pragma endregion

void MultiTexture::Initialize()
{
	CreateBuffers();
	CreateSRV();
	CreateRTV();
	CreateDSV();
	CreateGraphicsPipelineState();
}

void MultiTexture::Draw()
{
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	cmdList->SetPipelineState(pipelineState.Get());
	cmdList->SetGraphicsRootSignature(rootSignature.Get());
	// �v���~�e�B�u�`��̐ݒ�R�}���h
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // �O�p�`���X�g
	// �f�X�N���v�^�q�[�v�̐ݒ�R�}���h
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	cmdList->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeapSRV->GetGPUDescriptorHandleForHeapStart(), 0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	cmdList->SetGraphicsRootDescriptorTable(2,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeapSRV->GetGPUDescriptorHandleForHeapStart(), 1,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	// �`��R�}���h
	cmdList->DrawInstanced((UINT)vertices.size(), 1, 0, 0); // �S�Ă̒��_���g���ĕ`��
}

void MultiTexture::PreDrawScene()
{
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	for (size_t i = 0; i < 2; i++)
	{
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			texBuff[i].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
	}

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2]{};
	for (size_t i = 0; i < 2; i++)
	{
		rtvHandles[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapRTV->GetCPUDescriptorHandleForHeapStart(), i,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	}
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = descHeapDSV->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(2, rtvHandles, false, &dsvHandle);

	CD3DX12_VIEWPORT viewports[2]{};
	CD3DX12_RECT scissorRects[2]{};

	for (size_t i = 0; i < 2; i++)
	{
		viewports[i] = CD3DX12_VIEWPORT(0.0f, 0.0f, WindowsAPI::WIN_SIZE.x, WindowsAPI::WIN_SIZE.y);
		scissorRects[i] = CD3DX12_RECT(0, 0, (LONG)WindowsAPI::WIN_SIZE.x, (LONG)WindowsAPI::WIN_SIZE.y);
	}

	// �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	cmdList->RSSetViewports(2, viewports);
	cmdList->RSSetScissorRects(2, scissorRects); // �V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�

	for (size_t i = 0; i < 2; i++)
	{
		cmdList->ClearRenderTargetView(rtvHandles[i], CLEAR_COLOR, 0, nullptr);
	}
	cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void MultiTexture::PostDrawScene()
{
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	for (size_t i = 0; i < 2; i++)
	{
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			texBuff[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	}
}