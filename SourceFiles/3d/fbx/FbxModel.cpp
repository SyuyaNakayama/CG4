#include "FbxModel.h"
#include "D3D12Common.h"
#include "SpriteCommon.h"

void FbxModel::CreateBuffers()
{
#pragma region 頂点バッファ設定
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * vertices.size());
	CreateBuffer(&vertBuff, &vertMap, sizeVB);
	std::copy(vertices.begin(), vertices.end(), vertMap);

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);
#pragma endregion
#pragma region インデックスバッファ設定
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices.size());
	CreateBuffer(&indexBuff, &indexMap, sizeIB);
	std::copy(indices.begin(), indices.end(), indexMap);

	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;
#pragma endregion
#pragma region テクスチャバッファ設定
	Result result;
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);
	assert(img);

	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format, metadata.width, (UINT)metadata.height,
		(UINT16)metadata.arraySize, (UINT16)metadata.mipLevels);

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE, &texresDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&texBuff));

	result = texBuff->WriteToSubresource(0, nullptr, img->pixels, (UINT)img->rowPitch, (UINT)img->slicePitch);
#pragma endregion
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = texBuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(texBuff.Get(), &srvDesc, 
		SpriteCommon::GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
}

void FbxModel::Draw()
{
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	ID3D12DescriptorHeap* descHeap = SpriteCommon::GetDescriptorHeap();
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	cmdList->SetGraphicsRootDescriptorTable(1, descHeap->GetGPUDescriptorHandleForHeapStart());
	cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}
