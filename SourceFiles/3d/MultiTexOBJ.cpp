#include "MultiTexOBJ.h"
#include "D3D12Common.h"
#include "SpriteCommon.h"
using namespace Microsoft::WRL;
using namespace std;

// 静的メンバ変数の実体
ComPtr<ID3D12PipelineState> MultiTexOBJ::pipelinestate = nullptr;
ComPtr<ID3D12RootSignature> MultiTexOBJ::rootsignature = nullptr;

void MultiTexOBJ::StaticInitialize()
{
	PipelineManager pipelineManager;
	pipelineManager.LoadShaders(L"MultiTexObjectVS", L"MultiTexObjectPS");
	pipelineManager.AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	pipelineManager.AddInputLayout("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	pipelineManager.AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	pipelineManager.AddInputLayout("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipelineManager.InitDepthStencilState();
	pipelineManager.InitDSVFormat();
	pipelineManager.SetBlendDesc(D3D12_BLEND_OP_ADD, D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA);
	pipelineManager.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pipelineManager.AddRootParameter(PipelineManager::RootParamType::DescriptorTable);
	for (size_t i = 0; i < 3; i++) { pipelineManager.AddRootParameter(PipelineManager::RootParamType::CBV); }
	pipelineManager.AddRootParameter(PipelineManager::RootParamType::DescriptorTable, 1);
	pipelineManager.AddRootParameter(PipelineManager::RootParamType::DescriptorTable, 2);
	pipelineManager.CreatePipeline(pipelinestate, rootsignature);
}

void MultiTexOBJ::SetMainTex(std::unique_ptr<Sprite> mainTex)
{
	model->SetSprite(std::move(mainTex));
}

void MultiTexOBJ::Initialize(const std::string& modelName, const std::string& subTexName, const std::string& maskTexName, bool smoothing)
{
	model = Model::Create(modelName, smoothing);
	subTex = Sprite::Create(subTexName);
	maskTex = Sprite::Create(maskTexName);
}

void MultiTexOBJ::Draw(const WorldTransform& worldTransform)
{
	// コマンドリストをセット
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();

	// パイプラインステートの設定
	cmdList->SetPipelineState(pipelinestate.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootsignature.Get());
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// カメラ
	cmdList->SetGraphicsRootConstantBufferView(3, WorldTransform::GetViewProjection()->constBuffer->GetGPUVirtualAddress());
	SpriteCommon* spCommon = SpriteCommon::GetInstance();
	ID3D12DescriptorHeap* ppHeaps[] = { spCommon->GetDescriptorHeap() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	cmdList->SetGraphicsRootDescriptorTable(4, spCommon->GetGpuHandle(subTex->GetTextureIndex()));
	cmdList->SetGraphicsRootDescriptorTable(5, spCommon->GetGpuHandle(maskTex->GetTextureIndex()));
	model->Draw(worldTransform);
}