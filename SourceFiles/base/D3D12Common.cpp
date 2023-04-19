#include "D3D12Common.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
using namespace std;

D3D12_INPUT_ELEMENT_DESC SetInputLayout(LPCSTR semanticName, DXGI_FORMAT format)
{
	D3D12_INPUT_ELEMENT_DESC inputLayout =
	{
		semanticName, 0, format, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	};
	return inputLayout;
}

void LoadShader(ID3DBlob** shaderBlob, wstring shaderName, LPCSTR target)
{
	ID3DBlob* errorBlob = nullptr;

	wstring fileName = L"Resources/shaders/" + shaderName + L".hlsl";
	//シェーダの読み込みとコンパイル
	HRESULT result = D3DCompileFromFile(
		fileName.c_str(), // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", target, // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		shaderBlob, &errorBlob);
	// エラーなら
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		string error;
		error.resize(errorBlob->GetBufferSize());
		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}
}

void PipelineManager::CreatePipeline(ComPtr<ID3D12PipelineState>& pipelinestate, ComPtr<ID3D12RootSignature>& rootsignature)
{
	// グラフィックスパイプラインの流れを設定
	pipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	pipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
	if (gsBlob) { pipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get()); }

	// サンプルマスク
	pipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	pipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	// レンダーターゲットのブレンド設定
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	pipeline.BlendState.RenderTarget[0] = blenddesc;

	// 頂点レイアウトの設定
	pipeline.InputLayout.pInputElementDescs = inputLayout.data();
	pipeline.InputLayout.NumElements = (UINT)inputLayout.size();

	pipeline.NumRenderTargets = 1;	// 描画対象は1つ
	pipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	pipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Init_1_0(rootParams.size(), rootParams.data(), 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob, errorBlob;
	// バージョン自動判定のシリアライズ
	Result result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ルートシグネチャの生成
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));

	pipeline.pRootSignature = rootsignature.Get();

	// グラフィックスパイプラインの生成
	result = device->CreateGraphicsPipelineState(&pipeline, IID_PPV_ARGS(&pipelinestate));
}

void PipelineManager::AddRootParameter(RootParamType paramType)
{
	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER rootParam{};
	switch (paramType)
	{
	case RootParamType::CBV:
		rootParam.InitAsConstantBufferView(shaderRegister++);
		break;
	case RootParamType::DescriptorTable:
		rootParam.InitAsDescriptorTable(1, &descriptorRange);
		break;
	}
	rootParams.push_back(rootParam);
}

void PipelineManager::AddInputLayout(LPCSTR semanticName, DXGI_FORMAT format)
{
	inputLayout.push_back(SetInputLayout(semanticName, format));
}

void PipelineManager::SetBlendDesc(D3D12_BLEND_OP blendOp, D3D12_BLEND SrcBlend, D3D12_BLEND DestBlend)
{
	blenddesc.BlendOp = blendOp;
	blenddesc.SrcBlend = SrcBlend;
	blenddesc.DestBlend = DestBlend;
}

void PipelineManager::LoadShaders(std::wstring vsShaderName, std::wstring psShaderName, std::wstring gsShaderName)
{
	LoadShader(&vsBlob, vsShaderName, "vs_5_0");
	LoadShader(&psBlob, psShaderName, "ps_5_0");
	if (!gsShaderName.empty()) { LoadShader(&gsBlob, gsShaderName, "gs_5_0"); }
}