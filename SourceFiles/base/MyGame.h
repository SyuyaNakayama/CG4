#pragma once
#include "Framework.h"
#include "PostEffect.h"
#include "ViewProjection.h"

class MyGame : public Framework
{
private:
	std::array<PostEffect, 2> postEffects;
	ViewProjection viewProjection;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
};