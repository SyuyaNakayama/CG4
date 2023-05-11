﻿#include "MyGame.h"
#include "Model.h"
#include "ImGuiManager.h"
#include "FbxLoader.h"
#include "FbxObject3d.h"

void MyGame::Initialize()
{
	Framework::Initialize();
	sceneManager->SetNextScene(Scene::Normal, false);
	Model::StaticInitialize();
	FbxLoader::Initialize();
	FbxObject3d::CreateGraphicsPipeline();
	ImGuiManager::Initialize();
	viewProjection.Initialize();
	WorldTransform::SetViewProjection(&viewProjection);
	postEffect = std::make_unique<PostEffect>();
	postEffect->Initialize();
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
	FbxLoader::Finalize();
	Framework::Finalize();
}