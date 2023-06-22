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
	for (size_t i = 0; i < 2; i++)
	{
		postEffects[i].Initialize();
	}
	postEffects[0].SetEffectType(1);
	postEffects[1].SetEffectType(0);
	multiTexture.Initialize();
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

	multiTexture.PreDrawScene();
	sceneManager->Draw();
	multiTexture.PostDrawScene();

	static int i = 0;
	if (input->GetInstance()->IsTrigger(Key::_1)) { i = (i + 1) % 2; }

	dxCommon->PreDraw();
	if (i == 0) { postEffects[0].Draw(); }
	else { multiTexture.Draw(); }
	ImGuiManager::Draw();
	dxCommon->PostDraw();
}

void MyGame::Finalize()
{
	ImGuiManager::Finalize();
	Framework::Finalize();
}