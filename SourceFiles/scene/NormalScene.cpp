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
	// �J�����������
	if (ImGui::CollapsingHeader("Operate"))
	{
		// �J�����̉�荞��
		ImGui::Text("Left Drag : Camera Rotate");
		// �J�����̈ړ�
		ImGui::Text("Wheel Drag : Camera Move");
		// �����_�܂ł̋���
		ImGui::Text("Wheel Move : Camera Distance Change");
		// ���[�h�؂�ւ�
		ImGui::Text("Space Key : Mode Change");
	}
	// ���s�����̑���
	if (ImGui::CollapsingHeader("DirectionalLight"))
	{
		// �����̌���
		ImGuiManager::DragVector("DirLightDir", lightDir, 0.01f);
		// �����̐F
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
