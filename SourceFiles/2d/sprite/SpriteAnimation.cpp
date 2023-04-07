#include "SpriteAnimation.h"

void SpriteAnimation::Initialize(const std::string& textureName, size_t spriteWidth, int animationIntervel)
{
	sprite = Sprite::Create(textureName);
	width = spriteWidth;
	interval = animationIntervel;
	animeNumMax = (size_t)sprite->GetSize().x / width;
	sprite->SetSize({ (float)width,sprite->GetSize().y });
	sprite->SetTextureSize({ (float)width,sprite->GetTextureSize().y });
	sprite->Update();
}

void SpriteAnimation::Update()
{
	if (!interval.CountDown()) { return; }
	if (++animeNum >= animeNumMax) { animeNum = 0; }
	sprite->SetTextureLeftTop({ (float)animeNum * width ,0 });
	sprite->Update();
}