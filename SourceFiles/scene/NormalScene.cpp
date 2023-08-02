#include "NormalScene.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include <imgui.h>

void NormalScene::Initialize()
{
	debugCamera.Initialize({}, 10);
	WorldTransform::SetViewProjection(&debugCamera.GetViewProjection());
	worldTransform.Initialize();
	mulModel.Initialize("cube", "AlphaMask/Dirt.jpg", "AlphaMask/FirldMask.png");
	Model* m = mulModel.GetModel();
	m->SetSprite(Sprite::Create("AlphaMask/Grass.jpg"));
	model = Model::Create("cube");
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
	}
	if (sceneNum == 0)
	{
		ImGuiManager::InputVector("MainTexUVOffset", texProps[0].offset);
		ImGuiManager::InputVector("MainTexTiling", texProps[0].tiling);
		ImGuiManager::InputVector("SubTexUVOffset", texProps[1].offset);
		ImGuiManager::InputVector("SubTexTiling", texProps[1].tiling);
		ImGuiManager::InputVector("MaskTexUVOffset", texProps[2].offset);
		ImGuiManager::InputVector("MaskTexTiling", texProps[2].tiling);
		for (size_t i = 0; i < 3; i++) { mulModel.SetTexProp(i, texProps[i]); }
	}
	else
	{
		ImGui::SliderFloat("Dissolve", &worldTransform.dissolve, 0, 1);
	}

	worldTransform.Update();

	const char* items[] = { "TextureBlend", "Dissolve & Noise" };
	ImGui::Combo("Scene", &sceneNum, items, IM_ARRAYSIZE(items));
}

void NormalScene::Draw()
{
	if (sceneNum == 0)
	{
		mulModel.Draw(worldTransform);
	}
	else
	{
		Model::PreDraw();
		model->Draw(worldTransform);
	}
}
