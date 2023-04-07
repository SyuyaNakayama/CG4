#pragma once
#include <string>
#include "Sprite.h"
#include "Timer.h"

class SpriteAnimation
{
private:
	std::unique_ptr<Sprite> sprite;
	size_t width;
	Timer interval;
	size_t animeNum = 0;
	size_t animeNumMax = 0;

public:
	void Initialize(const std::string& textureName, size_t spriteWidth, int animationIntervel);
	void Update();
	void Draw() { sprite->Draw(); }
};