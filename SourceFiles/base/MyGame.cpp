#include "MyGame.h"
#include "Model.h"
#include "ImGuiManager.h"
#include "CollisionManager.h"
#include <future>

void MyGame::Initialize()
{
	Framework::Initialize();
	sceneManager->SetNextScene(Scene::Play, false);
	Model::InitializeGraphicsPipeline();
	ImGuiManager::Initialize();
	postEffect = std::make_unique<PostEffect>();
	postEffect->Initialize();
}

void MyGame::Update()
{
	ImGuiManager::Begin();
	Framework::Update();
	CollisionManager::CheckAllCollisions();
	WorldTransform::CameraUpdate();
	ImGuiManager::End();
}

void MyGame::Draw()
{
	postEffect->PreDrawScene();
	sceneManager->Draw();
	postEffect->PostDrawScene();

	dxCommon->PreDraw();
	postEffect->Draw();
	ImGuiManager::Draw();
	dxCommon->PostDraw();
}

void MyGame::Finalize()
{
	ImGuiManager::Finalize();
	Framework::Finalize();
}