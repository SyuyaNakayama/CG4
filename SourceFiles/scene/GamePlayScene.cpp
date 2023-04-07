#include "GamePlayScene.h"
#include <imgui.h>
#include "SpriteCommon.h"
#include "ImGuiManager.h"
#include "WindowsAPI.h"

void GamePlayScene::Initialize()
{
	lightGroup = LightGroup::Create();
	for (size_t i = 0; i < LightGroup::DIR_LIGHT_NUM; i++)
	{
		//lightGroup->SetDirLightActive(i, false);
	}
	debugCamera.Initialize();
	WorldTransform::SetViewProjection(&debugCamera.GetViewProjection());
	Model::SetLightGroup(lightGroup.get());

	skydome.Initialize(100.0f);
	stage.Initialize();
	player.Initialize(stage.GetStartPos());
	player.SetStageSize(stage.GetStageSize());
	input = Input::GetInstance();

	// WallクラスにPlayerのポインタを送る
	Wall::SetPlayerAddress(&player);

	//ステージ開始のカメラの初期位置
	viewProjection.target = stage.GetDoorPos();
	viewProjection.eye = stage.GetDoorPos() + Vector3{ 0,10,-15 };
	viewProjection.farZ = 1500.0f;
	viewProjection.Initialize();
}

void GamePlayScene::StartScene()
{
	if (!Gimmick::GetIsStart()) { return; }
	// カメラ補間開始
	float time = 100; // カメラが移動する時間
	float timeRate = ++timer / time;
	viewProjection.eye = Lerp(stage.GetDoorPos() + Vector3{ 0,10,-15 }, { 0,50,-75 }, timeRate);
	viewProjection.target = Lerp(stage.GetDoorPos(), {}, timeRate);
	if (timer < time) { return; }
	// カメラ補間終了
	timer = 0;
	Gimmick::SetIsStart(false);
	// チュートリアルなら操作説明を有効に
	if (Stage::GetStageNum() == (UINT16)Stage::StageNum::Tutorial) { UIUpdate = &GamePlayScene::UI_Camera; }
}

void GamePlayScene::Update()
{
	StartScene();
	skydome.Update();
	player.Update();
	debugCamera.Update();
	stage.Update();
	lightGroup->Update();
	// UIの調整
	if (UIUpdate) { (this->*UIUpdate)(); }

	if (WorldTransform::GetViewProjection() != &viewProjection && input->IsTrigger(Mouse::Right) && !player.IsCameraChange())
	{
		WorldTransform::SetViewProjection(&viewProjection);
	}
}

// 視点切り替えの操作説明
void GamePlayScene::UI_Camera()
{
	// スプライトの取得
	ui = UIDrawer::GetUI((size_t)4 + input->IsConnectGamePad());
	// スプライトの設定
	ui->SetIsInvisible(false);
	ui->SetPosition(WindowsAPI::WIN_SIZE / 2.0f);
	// マウスが右クリックされた時
	if (input->IsTrigger(Mouse::Right))
	{
		// UIを消して次の操作説明へ
		ui->SetIsInvisible(true);
		UIUpdate = &GamePlayScene::UI_Dark;
	}
}

// 火を消す操作説明
void GamePlayScene::UI_Dark()
{
	// プレイヤーが一定より手前ならスキップ
	if (player.GetWorldPosition().z < -8.0f) { return; }
	// スプライトの取得
	ui = UIDrawer::GetUI((size_t)0 + input->IsConnectGamePad());
	// スプライトの設定
	ui->SetPosition(WindowsAPI::WIN_SIZE / 2.0f);
	ui->SetColor({ 0,0,0,1 });
	ui->SetIsInvisible(false);
	// プレイヤーの火が青炎なら
	if (player.IsBlueFire())
	{
		// UIを消して操作説明を終わる
		ui->SetIsInvisible(true);
		UIUpdate = nullptr;
	}
}

void GamePlayScene::Draw()
{
	Model::PreDraw();
	//skydome.Draw();
	player.Draw();
	stage.Draw();
	Model::PostDraw();
}
