#include "FadeManager.h"
#include "WindowsAPI.h"

void FadeManager::Initialize()
{
	sprite = Sprite::Create("white1x1.png");
	sprite->SetSize(WindowsAPI::GetInstance()->WIN_SIZE);
}

void FadeManager::Update()
{
	if (!isFade) { return; }
	isChange = false;
	alpha += fadePerFlame;
	if (alpha >= 1.0f)
	{
		alpha = 1.0f;
		fadePerFlame = -fadePerFlame;
		isChange = true;
	}
	if (alpha <= 0)
	{
		alpha = 0;
		fadePerFlame = -fadePerFlame;
		isFade = false;
	}
	sprite->SetColor({ 0,0,0,alpha });
	sprite->Update();
}