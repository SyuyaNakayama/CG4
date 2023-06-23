#include "MyGame.h"
#include "Model.h"
#include "ImGuiManager.h"

void MyGame::Initialize()
{
	Framework::Initialize();
	sceneManager->SetNextScene(Scene::Normal, false);
	Model::StaticInitialize();
	ImGuiManager::Initialize();
	viewProjection.Initialize();
	WorldTransform::SetViewProjection(&viewProjection);
	for (size_t i = 0; i < 3; i++)
	{
		postEffects[i].Initialize();
	}
	for (size_t i = 0; i < 3; i++)
	{
		multiTextures[i].Initialize();
	}
	postEffects[0].SetEffectType(1);
	postEffects[1].SetEffectType(2);
	multiTextures[0].SetConstBufferData({ 1,1 });
	multiTextures[1].SetConstBufferData({ 2,1 });
	multiTextures[2].SetConstBufferData({ 1,0 });
}

void MyGame::Update()
{
	ImGuiManager::Begin();
	Framework::Update();
	WorldTransform::CameraUpdate();
	Model::LightUpdate();
	ImGuiManager::End();
}

void MyGame::Draw()
{
	postEffects[0].PreDrawScene();
	sceneManager->Draw();
	postEffects[0].PostDrawScene();

	postEffects[1].PreDrawScene();
	postEffects[0].Draw();
	postEffects[1].PostDrawScene();

	postEffects[2].PreDrawScene();
	postEffects[1].Draw();
	postEffects[2].PostDrawScene();

	//multiTextures[0].PreDrawScene();
	//sceneManager->Draw();
	//multiTextures[0].PostDrawScene();

	//multiTextures[1].PreDrawScene();
	//multiTextures[0].Draw();
	//multiTextures[1].PostDrawScene();

	//multiTextures[2].PreDrawScene();
	//multiTextures[1].Draw();
	//multiTextures[2].PostDrawScene();

	static int i = 0;
	if (input->GetInstance()->IsTrigger(Key::_1)) { i = (i + 1) % 2; }

	std::array<PostEffect, 2> postE = { postEffects[0],postEffects[2] };

	dxCommon->PreDraw();
	switch (i)
	{
	case 0: postEffects[1].Draw();	break;
	case 1: multiTextures[0].Draw(postE);	break;
	}
	ImGuiManager::Draw();
	dxCommon->PostDraw();
}

void MyGame::Finalize()
{
	ImGuiManager::Finalize();
	Framework::Finalize();
}