#pragma once
#include "BaseScene.h"
#include "DebugCamera.h"
#include "Model.h"

class NormalScene : public BaseScene
{
	DebugCamera debugCamera;
	std::array<std::unique_ptr<Model>, 4> spheres;
	std::array<WorldTransform, 4> sphereWTs;
	Vector3 lightDir = { 0,-1,0 };
	ColorRGB lightColor = { 1,1,1 };
	float rimPower = 5.0f;
	bool isRimSeparate = false;

public:
	void Initialize();
	void Update();
	void Draw();
};