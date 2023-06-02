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
	// シェーダの読み込みとコンパイル
	pManager.LoadShaders(L"FBXVS", L"FBXPS");
	// 頂点レイアウト
	pManager.AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	pManager.AddInputLayout("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	pManager.AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	// 影響を受けるボーン番号(4つ)
	pManager.AddInputLayout("BONEINDICES", DXGI_FORMAT_R32G32B32A32_UINT);
	// ボーンのスキンウェイト(4つ)
	pManager.AddInputLayout("BONEWEIGHTS", DXGI_FORMAT_R32G32B32A32_FLOAT);
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
	
	std::array<CD3DX12_DESCRIPTOR_RANGE, 3> descRangeSRVs{};
	for (UINT i = 0; i < descRangeSRVs.size(); i++) { descRangeSRVs[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i); }

	CD3DX12_ROOT_PARAMETER rootParam{};
	rootParam.InitAsDescriptorTable(descRangeSRVs.size(), descRangeSRVs.data());
	pManager.rootParams.push_back(rootParam);

	pManager.AddRootParameter(PipelineManager::RootParamType::CBV); // CBV（スキニング用）
	pManager.AddRootParameter(PipelineManager::RootParamType::CBV); // CBV（マテリアル用）
	pManager.AddRootParameter(PipelineManager::RootParamType::CBV); // CBV（ライト用）
	// グラフィックスパイプラインの生成
	pManager.CreatePipeline(pipelinestate, rootsignature);
}

void FbxObject3d::Initialize(WorldTransform* worldTransform, FbxModel* model)
{
	// 定数バッファの生成
	CreateBuffer(&constBuff, &constMap, (sizeof(ConstBufferData) + 0xff) & ~0xff);
	CreateBuffer(&constBuffSkin, &constMapSkin, (sizeof(ConstBufferDataSkin) + 0xff) & ~0xff);

	this->worldTransform = worldTransform;
	this->worldTransform->Initialize();
	this->model = model;

	for (int i = 0; i < MAX_BONES; i++) { constMapSkin->bones[i] = Matrix4::Identity(); }

	// 1フレーム分の時間を60FPSで設定
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);
	PlayAnimation();
}

void FbxObject3d::Update()
{
	// スケール、回転、平行移動行列の計算
	worldTransform->Update();

	// アニメーション
	if (isPlay)
	{
		// 1フレーム進める
		currentTime += frameTime;
		// 最後まで再生したら先頭に戻す
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
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootConstantBufferView(2, constBuffSkin->GetGPUVirtualAddress());
	lightGroup->Draw(4);
	// モデル描画
	model->Draw();
}

void FbxObject3d::PlayAnimation()
{
	FbxScene* fbxScene = model->GetFbxScene();
	// 0番のアニメーション取得
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	// アニメーションの名前取得
	const char* animstackname = animstack->GetName();
	// アニメーションの時間取得
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);
	// 開始時間取得
	startTime = takeinfo->mLocalTimeSpan.GetStart();
	// 終了時間取得
	endTime = takeinfo->mLocalTimeSpan.GetStop();
	// 開始時間に合わせる
	currentTime = startTime;
	// 再生中状態にする
	isPlay = true;
}
