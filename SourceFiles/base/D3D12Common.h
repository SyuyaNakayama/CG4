#pragma once
#include <string>
#include <memory>
#include <cassert>
#include "DirectXCommon.h"

template<class T> void CreateBuffer(ID3D12Resource** buff, T** map, UINT64 width)
{
	// バッファの生成
	Result result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(width),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(buff));

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	result = (*buff)->Map(0, nullptr, (void**)map);
	// 繋がりを解除
	(*buff)->Unmap(0, nullptr);
}

D3D12_INPUT_ELEMENT_DESC SetInputLayout(LPCSTR semanticName, DXGI_FORMAT format);
void LoadShader(ID3DBlob** shaderBlob, std::wstring shaderName, LPCSTR target);

// result値がエラーなら自動で警告が出る
struct Result
{
private:
	HRESULT result = S_OK;

public:
	Result() {}
	Result(const HRESULT& result) { operator=(result); }

	void operator=(HRESULT result_)
	{
		result = result_;
		assert(SUCCEEDED(result));
	}
};

struct PipelineManager
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline{};
	ComPtr<ID3DBlob> vsBlob, gsBlob, psBlob;
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	UINT shaderRegister = 0;
	std::vector<CD3DX12_ROOT_PARAMETER> rootParams;

public:
	enum class RootParamType { CBV, DescriptorTable };

	void LoadShaders(std::wstring vsShaderName, std::wstring psShaderName, std::wstring gsShaderName = L"");
	void AddInputLayout(LPCSTR semanticName, DXGI_FORMAT format);
	void AddRootParameter(RootParamType paramType);
	void SetBlendDesc(D3D12_BLEND_OP blendOp, D3D12_BLEND srcBlend, D3D12_BLEND destBlend);
	// 図形の形状設定
	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType) { pipeline.PrimitiveTopologyType = primitiveTopologyType; }
	// デプスステンシルステート
	void InitDepthStencilState() { pipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); }
	// 深度バッファのフォーマット
	void InitDSVFormat() { pipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT; }
	void SetDepthWriteMask(D3D12_DEPTH_WRITE_MASK depthWriteMask) { pipeline.DepthStencilState.DepthWriteMask = depthWriteMask; }
	void CreatePipeline(ComPtr<ID3D12PipelineState>& pipelinestate, ComPtr<ID3D12RootSignature>& rootsignature);
};