#include "NormalScene.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"

void NormalScene::Initialize()
{
	debugCamera.Initialize({},10);
	WorldTransform::SetViewProjection(&debugCamera.GetViewProjection());
	LightGroup* lightGroup = Model::GetLightGroup();
	for (size_t i = 1; i < 3; i++)
	{
		lightGroup->SetDirLightActive(i, false);
	}
	sphere = Model::Create("sphere", true);
	sphere->GetSprite()->SetColor({ 1,0,0,1 });
	sphere->Update();
	sphereWT.Initialize();
}

void NormalScene::Update()
{
	debugCamera.Update();
	sphereWT.Update();
}

void NormalScene::Draw()
{
	Model::PreDraw();
	sphere->Draw(sphereWT);
	Model::PostDraw();
}
