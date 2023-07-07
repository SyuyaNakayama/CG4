#include "NormalScene.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include <imgui.h>

void NormalScene::Initialize()
{
	debugCamera.Initialize({}, 10);
	WorldTransform::SetViewProjection(&debugCamera.GetViewProjection());
	LightGroup* lightGroup = Model::GetLightGroup();
	for (size_t i = 1; i < 3; i++)
	{
		lightGroup->SetDirLightActive(i, false);
	}
	model = Model::Create("cube");
	for (auto& w : worldTransforms) { w.Initialize(); }
	for (size_t i = 0; i < worldTransforms.size(); i++)
	{
		worldTransforms[i].translation.x = -4.5f + 3.0f * (float)i;
	}
	mulModel.Initialize("cube", "AlphaMask/Dirt.jpg", "AlphaMask/FirldMask.png");
	Model* m = mulModel.GetModel();
	m->SetSprite(Sprite::Create("AlphaMask/Grass.jpg"));
}

void NormalScene::Update()
{
	debugCamera.Update();
	// カメラ操作説明
	if (ImGui::CollapsingHeader("Operate"))
	{
		// カメラの回り込み
		ImGui::Text("Left Drag : Camera Rotate");
		// カメラの移動
		ImGui::Text("Wheel Drag : Camera Move");
		// 注視点までの距離
		ImGui::Text("Wheel Move : Camera Distance Change");
		// モード切り替え
		ImGui::Text("Space Key : Mode Change");
	}
	// 平行光源の操作
	if (ImGui::CollapsingHeader("DirectionalLight"))
	{
		// 光源の向き
		ImGuiManager::DragVector("DirLightDir", lightDir, 0.01f);
		// 光源の色
		ImGuiManager::ColorEdit("DirLight", lightColor);
	}
	Model::GetLightGroup()->SetDirLightColor(0, lightColor);
	Model::GetLightGroup()->SetDirLightDir(0, lightDir);
	for (auto& w : worldTransforms) { w.Update(); }
}

void NormalScene::Draw()
{
	mulModel.Draw(worldTransforms[0]);
	Model::PreDraw();
	//for (size_t i = 0; i < worldTransforms.size(); i++) { model->Draw(worldTransforms[i]); }
	Model::PostDraw();
}
