#include "NormalScene.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"

void NormalScene::Initialize()
{
	debugCamera.Initialize({}, 3);
	lightGroup = LightGroup::Create();
	WorldTransform::SetViewProjection(&debugCamera.GetViewProjection());
	FbxObject3d::CreateGraphicsPipeline();
	fbxModel_ = FbxLoader::LoadModelFromFile("spherePBR");
	fbxObject_ = new FbxObject3d;
	fbxObject_->Initialize(&fbxObjWT, fbxModel_);
	FbxObject3d::SetLightGroup(lightGroup.get());
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
