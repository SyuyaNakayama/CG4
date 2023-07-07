#pragma once
#include "BaseScene.h"
#include "DebugCamera.h"
#include "MultiTexOBJ.h"

class NormalScene : public BaseScene
{
	DebugCamera debugCamera;
	std::unique_ptr<Model> model;
	MultiTexOBJ mulModel;
	std::array<WorldTransform, 4> worldTransforms;
	Vector3 lightDir = { 0,0,1 };
	ColorRGB lightColor = { 1,1,1 };
	float rimPower = 5.0f;
	bool isRimSeparate = false;

public:
	void Initialize();
	void Update();
	void Draw();
};