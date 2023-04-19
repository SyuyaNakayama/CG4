#include "NormalScene.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"

void NormalScene::Initialize()
{
	debugCamera.Initialize();
	WorldTransform::SetViewProjection(&debugCamera.GetViewProjection());
	FbxLoader::GetInstance()->Initialize(DirectXCommon::GetInstance()->GetDevice());
	FbxObject3d::SetDevice(DirectXCommon::GetInstance()->GetDevice());
	FbxObject3d::SetViewProjection(&debugCamera.GetViewProjection());
	FbxObject3d::CreateGraphicsPipeline();
	fbxModel_ = FbxLoader::GetInstance()->LoadModelFromFile("cube");
	fbxObject_ = new FbxObject3d;
	fbxObject_->Initialize(&fbxObjWT);
	fbxObject_->SetModel(fbxModel_);
}

void NormalScene::Update()
{
	debugCamera.Update();
	fbxObject_->Update();
}

void NormalScene::Draw()
{
	fbxObject_->Draw(DirectXCommon::GetInstance()->GetCommandList());
}
