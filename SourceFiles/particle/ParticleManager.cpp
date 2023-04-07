#include <random>
#include "ParticleManager.h"
#include "SpriteCommon.h"
#include "D3D12Common.h"
#include "WorldTransform.h"
using namespace Microsoft::WRL;

// 静的メンバ変数の実体
ComPtr<ID3D12RootSignature> ParticleManager::rootsignature;
ComPtr<ID3D12PipelineState> ParticleManager::pipelinestate;
ComPtr<ID3D12Resource> ParticleManager::vertBuff;
ParticleManager::VertexPos* ParticleManager::vertMap = nullptr;
ComPtr<ID3D12Resource> ParticleManager::constBuff;
ParticleManager::ConstBufferData* ParticleManager::constMap = nullptr;
D3D12_VERTEX_BUFFER_VIEW ParticleManager::vbView{};
uint32_t ParticleManager::textureIndex = 0;
DiffuseParticle ParticleManager::diffuseParticle;
DirectionalParticle ParticleManager::directionalParticle;

void ParticleManager::Initialize()
{
	// パイプライン初期化
	InitializeGraphicsPipeline();
	// テクスチャ読み込み
	textureIndex = SpriteCommon::GetInstance()->LoadTexture("Particle.png", 1);
	// モデル生成
	CreateBuffers();
}

void ParticleManager::InitializeGraphicsPipeline()
{
	PipelineManager pipelineManager;
	pipelineManager.LoadShaders(L"ParticleVS", L"ParticlePS", L"ParticleGS");
	pipelineManager.AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	pipelineManager.AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32B32_FLOAT);
	pipelineManager.InitDepthStencilState();
	pipelineManager.SetBlendDesc(D3D12_BLEND_OP_ADD, D3D12_BLEND_ONE, D3D12_BLEND_ONE); // 光パーティクル
	//pipelineManager.SetBlendDesc(D3D12_BLEND_OP_REV_SUBTRACT, D3D12_BLEND_ONE, D3D12_BLEND_ONE); // 闇パーティクル
	pipelineManager.InitDSVFormat();
	pipelineManager.SetDepthWriteMask(D3D12_DEPTH_WRITE_MASK_ZERO);
	pipelineManager.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);
	pipelineManager.AddRootParameter(PipelineManager::RootParamType::CBV);
	pipelineManager.AddRootParameter(PipelineManager::RootParamType::DescriptorTable);
	pipelineManager.CreatePipeline(pipelinestate, rootsignature);
}

void ParticleManager::CreateBuffers()
{
	CreateBuffer(&vertBuff, &vertMap, PARTICLE_MAX * sizeof(VertexPos));

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = PARTICLE_MAX * sizeof(VertexPos);
	vbView.StrideInBytes = sizeof(VertexPos);

	CreateBuffer(&constBuff, &constMap, (sizeof(ConstBufferData) + 0xff) & ~0xff);
}

void ParticleManager::Update()
{
	diffuseParticle.Update();
	directionalParticle.Update();

	// 定数バッファへデータ転送
	std::list<DiffuseParticle::Particle> diffuse = diffuseParticle.GetParticles();
	std::list<DirectionalParticle::Particle> directional = directionalParticle.GetParticles();
	int i = 0;

	for (auto& dif : diffuse)
	{
		vertMap[i].pos = dif.position;
		vertMap[i++].scale = dif.scale;
	}
	for (auto& dir : directional)
	{
		vertMap[i].pos = dir.position;
		vertMap[i++].scale = dir.scale;
	}

	// 定数バッファへデータ転送
	constMap->mat = WorldTransform::GetViewProjection()->GetViewProjectionMatrix();
	constMap->matBillboard = Matrix4::GetBillboard();
}

void ParticleManager::Draw()
{
	// コマンドリストをセット
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	// パイプラインステートの設定
	cmdList->SetPipelineState(pipelinestate.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootsignature.Get());
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	// 頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	// デスクリプタヒープの配列
	SpriteCommon* spCommon = SpriteCommon::GetInstance();
	ID3D12DescriptorHeap* ppHeaps[] = { spCommon->GetDescriptorHeap() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	// シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1, spCommon->GetGpuHandle(textureIndex));
	// 描画コマンド
	cmdList->DrawInstanced((UINT)AllParticleNum(), 1, 0, 0);
}

void ParticleManager::Clear()
{
	diffuseParticle.Clear();
	directionalParticle.Clear();
}

void ParticleManager::Add(const DiffuseParticle::AddProp& particleProp)
{
	if (IsParticleMax()) { return; }
	diffuseParticle.Add(particleProp);
}

void ParticleManager::Add(const DirectionalParticle::AddProp& particleProp)
{
	if (IsParticleMax()) { return; }
	directionalParticle.Add(particleProp);
}