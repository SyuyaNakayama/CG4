#include "NormalScene.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"

static ColorRGB baseColor;
static float metalness;
static float specular;
static float roughness;

void NormalScene::Initialize()
{
	debugCamera.Initialize({}, 3);
	lightGroup = LightGroup::Create();
	WorldTransform::SetViewProjection(&debugCamera.GetViewProjection());
	FbxObject3d::CreateGraphicsPipeline();
	fbxModel_ = FbxLoader::LoadModelFromFile("spherePBR");
	fbxObject_ = new FbxObject3d;
	fbxObject_->Initialize(&fbxObjWT, fbxModel_);
	FbxObject3d::SetLightGroup(lightGroup.get());

	baseColor = fbxModel_->GetBaseColor();
	metalness = fbxModel_->GetMetalness();
	specular = fbxModel_->GetSpecular();
	roughness = fbxModel_->GetRoughness();
}

void NormalScene::Update()
{
	ImGui::Begin("Material");
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(300, 130));
	ImGuiManager::ColorEdit("baseColor", baseColor);
	ImGui::SliderFloat("metalness", &metalness, 0, 1);
	ImGui::SliderFloat("specular", &specular, 0, 1);
	ImGui::SliderFloat("roughness", &roughness, 0, 1);
	ImGui::End();

	debugCamera.Update();
	fbxModel_->SetBaseColor(baseColor);
	fbxModel_->SetMetalness(metalness);
	fbxModel_->SetSpecular(specular);
	fbxModel_->SetRoughness(roughness);
	fbxModel_->TransferMaterial();
	fbxObject_->Update();
}

void NormalScene::Draw()
{
	fbxObject_->Draw();
}
