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
	for (auto& postEffect : tempTextures) { postEffect.Initialize(); }
	multiTextures.Initialize();
	postEffects[0].SetEffectType(1); // 高輝度抽出
	postEffects[1].SetEffectType(2); // ガウシアンブラー
	postEffects[2].SetEffectType(3); // 線形ガウシアンブラー
	postEffects[2].SetAngle(45.0f * PI / 180.0f);
	postEffects[3].SetEffectType(3); // 線形ガウシアンブラー
	postEffects[3].SetAngle(135.0f * PI / 180.0f);
}

void MyGame::Update()
{
	ImGuiManager::Begin();
	Framework::Update();
	WorldTransform::CameraUpdate();
	Model::LightUpdate();

	if (input->GetInstance()->IsTrigger(Key::Space)) { mode = (mode + 1) % 3; }
	switch (mode)
	{
	case 0: ImGui::Text("Mode : GaussianBlur"); break;
	case 1: ImGui::Text("Mode : Bloom"); break;
	case 2: ImGui::Text("Mode : Cross Filter"); break;
	}
	ImGuiManager::End();
}

void MyGame::Draw()
{
	std::array<PostEffect, 2> postE;

	switch (mode)
	{
	case 0:	// ガウシアンブラー
		postEffects[1].PreDrawScene();
		sceneManager->Draw();
		postEffects[1].PostDrawScene();
		break;
	case 1:	// ブルーム
		postEffects[0].PreDrawScene();
		sceneManager->Draw();
		postEffects[0].PostDrawScene();

		postEffects[1].PreDrawScene();
		postEffects[0].Draw();
		postEffects[1].PostDrawScene();

		tempTextures[0].PreDrawScene();
		postEffects[1].Draw();
		tempTextures[0].PostDrawScene();
		break;
	case 2: // クロスフィルタ
		postEffects[0].PreDrawScene();
		sceneManager->Draw();
		postEffects[0].PostDrawScene();

		for (size_t i = 1; i < tempTextures.size(); i++)
		{
			postEffects[i + 1].PreDrawScene();
			postEffects[0].Draw();
			postEffects[i + 1].PostDrawScene();

			tempTextures[i].PreDrawScene();
			postEffects[i + 1].Draw();
			tempTextures[i].PostDrawScene();
		}

		postE = { tempTextures[1],tempTextures[2] };

		tempTextures[0].PreDrawScene();
		multiTextures.Draw(postE);
		tempTextures[0].PostDrawScene();
		break;
	}

	postE = { postEffects[0],tempTextures[0] };

	dxCommon->PreDraw();
	switch (mode)
	{
	case 0:	postEffects[1].Draw(); break;
	case 1:
	case 2:	multiTextures.Draw(postE); break;
	}
	ImGuiManager::Draw();
	dxCommon->PostDraw();
}

void MyGame::Finalize()
{
	ImGuiManager::Finalize();
	Framework::Finalize();
}