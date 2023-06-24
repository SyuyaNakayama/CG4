#include "MyGame.h"
#include "Model.h"
#include "ImGuiManager.h"
#include <imgui.h>

void MyGame::Initialize()
{
	Framework::Initialize();
	sceneManager->SetNextScene(Scene::Normal, false);
	Model::StaticInitialize();
	ImGuiManager::Initialize();
	viewProjection.Initialize();
	WorldTransform::SetViewProjection(&viewProjection);
	PostEffect::StaticInitialize();
	for (auto& postEffect : postEffects) { postEffect.Initialize(); }
	multiTextures.Initialize();
	postEffects[0].SetEffectType(1);
	postEffects[1].SetEffectType(2);
	postEffects[3].SetEffectType(2);
}

void MyGame::Update()
{
	ImGuiManager::Begin();
	Framework::Update();
	WorldTransform::CameraUpdate();
	Model::LightUpdate();

	if (input->GetInstance()->IsTrigger(Key::Space)) { mode = (mode + 1) % 2; }
	switch (mode)
	{
	case 0: ImGui::Text("Mode : GaussianBlur"); break;
	case 1: ImGui::Text("Mode : Bloom"); break;
	}
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

	postEffects[3].PreDrawScene();
	sceneManager->Draw();
	postEffects[3].PostDrawScene();

	std::array<PostEffect, 2> postE = { postEffects[0],postEffects[2] };

	dxCommon->PreDraw();
	switch (mode)
	{
	case 0: postEffects[3].Draw(); break;
	case 1: multiTextures.Draw(postE); break;
	}
	ImGuiManager::Draw();
	dxCommon->PostDraw();
}

void MyGame::Finalize()
{
	ImGuiManager::Finalize();
	Framework::Finalize();
}