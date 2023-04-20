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
	// シェーダの読み込みとコンパイル
	pManager.LoadShaders(L"FBXVS", L"FBXPS");
	// 頂点レイアウト
	pManager.AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	pManager.AddInputLayout("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	pManager.AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	// レンダーターゲットのブレンド設定
	pManager.SetBlendDesc(D3D12_BLEND_OP_ADD, D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA);
	// デプスステンシルステート
	pManager.InitDepthStencilState();
	// 深度バッファのフォーマット
	pManager.InitDSVFormat();
	// 図形の形状設定（三角形）
	pManager.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	// ルートパラメータ
	pManager.AddRootParameter(PipelineManager::RootParamType::CBV); // CBV（座標変換行列用）
	pManager.AddRootParameter(PipelineManager::RootParamType::DescriptorTable); // SRV（テクスチャ）
	// グラフィックスパイプラインの生成
	pManager.CreatePipeline(pipelinestate, rootsignature);
}

void FbxObject3d::Initialize(WorldTransform* worldTransform)
{
	// 定数バッファの生成
	CreateBuffer(&constBuffTransform, &constMap, (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff);

	this->worldTransform = worldTransform;
	this->worldTransform->Initialize();
}

void FbxObject3d::Update()
{
	// スケール、回転、平行移動行列の計算
	worldTransform->Update();

	// 定数バッファへデータ転送
	constMap->viewproj = WorldTransform::GetViewProjection()->GetViewProjectionMatrix();
	constMap->world = model->GetModelTransform() * worldTransform->matWorld;
	constMap->cameraPos = WorldTransform::GetViewProjection()->eye;
}

void FbxObject3d::Draw()
{
	// モデルの割り当てがなければ描画しない
	if (model == nullptr) { return; }

	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	// パイプラインステートの設定
	cmdList->SetPipelineState(pipelinestate.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootsignature.Get());
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffTransform->GetGPUVirtualAddress());

	// モデル描画
	model->Draw();
}