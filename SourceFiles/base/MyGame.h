#pragma once
#include "Framework.h"
#include "PostEffect.h"
#include "ViewProjection.h"

class MyGame : public Framework
{
private:
	std::unique_ptr<PostEffect> postEffect;
	ViewProjection viewProjection;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
};