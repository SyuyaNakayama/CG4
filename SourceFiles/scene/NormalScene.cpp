#include "NormalScene.h"
#include "FbxLoader.h"
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
	for (auto& sphere : spheres)
	{
		sphere = Model::Create("sphere", true);
		sphere->GetSprite()->SetColor({ 1,0,0,1 });
	}
	spheres[0]->SetIsToon(true);
	spheres[2]->SetIsUseRim(true);
	spheres[3]->SetSprite(Sprite::Create("09_Test_Texture2.jpg"));
	for (int i = 0; i < sphereWTs.size(); i++)
	{
		sphereWTs[i].Initialize();
		sphereWTs[i].translation.x = 3.0f * ((float)i - 1.5f);
	}
}

void NormalScene::Update()
{
	debugCamera.Update();
	// カメラ操作説明
	if (ImGui::CollapsingHeader("Camera"))
	{
		// カメラの回り込み
		ImGui::Text("Left Drag : Camera Rotate");
		// カメラの移動
		ImGui::Text("Wheel Drag : Camera Move");
		// 注視点までの距離
		ImGui::Text("Wheel Move : Camera Distance Change");
	}
	// 平行光源の操作
	if (ImGui::CollapsingHeader("DirectionalLight"))
	{
		// 光源の向き
		ImGuiManager::DragVector("DirLightDir", lightDir, 0.01f);
		// 光源の色
		ImGuiManager::ColorEdit("DirLight", lightColor);
	}
	// リムライトの操作
	if (ImGui::CollapsingHeader("RimLight"))
	{
		// リムライトの強さ
		ImGui::SliderFloat("RimPower", &rimPower, 0, 8.0f);
		// 輪郭線の切り替え
		ImGui::Checkbox("isRimSeparate", &isRimSeparate);
	}
	// ImGuiで変えた値の代入
	Model::GetLightGroup()->SetDirLightColor(0, lightColor);
	Model::GetLightGroup()->SetDirLightDir(0, lightDir);
	spheres[2]->SetRimPower(rimPower);
	spheres[2]->SetIsRimSeparate(isRimSeparate);
	// オブジェクトの更新
	for (auto& sphere : spheres) { sphere->Update(); }
	for (auto& sphereWT : sphereWTs) { sphereWT.Update(); }
}

void NormalScene::Draw()
{
	Model::PreDraw();
	for (size_t i = 0; i < spheres.size(); i++) { spheres[i]->Draw(sphereWTs[i]); }
	Model::PostDraw();
}
