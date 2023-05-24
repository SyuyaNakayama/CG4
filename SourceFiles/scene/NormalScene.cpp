#include "NormalScene.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"

void NormalScene::Initialize()
{
	debugCamera.Initialize();
	WorldTransform::SetViewProjection(&debugCamera.GetViewProjection());
	FbxObject3d::CreateGraphicsPipeline();
	fbxModel_ = FbxLoader::LoadModelFromFile("boneTest");
	fbxObject_ = new FbxObject3d;
	fbxObject_->Initialize(&fbxObjWT, fbxModel_);
	fbxObject_->PlayAnimation();
}

void NormalScene::Update()
{
	debugCamera.Update();
	fbxObject_->Update();
}

void NormalScene::Draw()
{
	fbxObject_->Draw();
}
