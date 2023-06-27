#include "Framework.h"
#include "SpriteCommon.h"

void Framework::Initialize()
{
	wAPI->Initialize(L"DirectX12_CG4_CG5_Test");
	dxCommon->Initialize();
	SpriteCommon::GetInstance()->Initialize();
	input->Initialize();
	sceneManager->Initialize();
}

void Framework::Update()
{
	input->Update();
	sceneManager->Update();
}

void Framework::Finalize()
{
	sceneManager->Finalize();
	wAPI->Finalize();
}

bool Framework::IsEndRequest()
{
	// Ｘボタンで終了メッセージが来たらゲームループを抜ける
	if (wAPI->ProcessMessage()) { return true; }
	// ESCキーを押したらゲームループを抜ける
	if (input->IsTrigger(Key::Escape)) { return true; }
	return false;
}

void Framework::Run()
{
	Initialize();

	while (!IsEndRequest())
	{
		Update();
		Draw();
	}

	Finalize();
}