#pragma once
#include "Framework.h"
#include "PostEffect.h"
#include "MultiTexture.h"
#include "ViewProjection.h"

class MyGame : public Framework
{
private:
	std::array<PostEffect, 3> postEffects;
	MultiTexture multiTextures[3];
	ViewProjection viewProjection;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
};