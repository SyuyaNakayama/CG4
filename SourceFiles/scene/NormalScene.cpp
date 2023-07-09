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
	worldTransform.Update();
	maskTexProp.offset.y = fmod(maskTexProp.offset.y + 0.03f, 1.0f);
	mulModel.SetTexProp(2, maskTexProp);
}

void NormalScene::Draw()
{
	mulModel.Draw(worldTransform);
}
