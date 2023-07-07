#include "MyGame.h"
#include "MultiTexOBJ.h"
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
	MultiTexOBJ::StaticInitialize();
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
	dxCommon->PreDraw();
	sceneManager->Draw();
	ImGuiManager::Draw();
	dxCommon->PostDraw();
}

void MyGame::Finalize()
{
	ImGuiManager::Finalize();
	Framework::Finalize();
}