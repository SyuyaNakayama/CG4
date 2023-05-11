#pragma once
#include "BaseScene.h"
#include "DebugCamera.h"
#include "Model.h"

class NormalScene : public BaseScene
{
	DebugCamera debugCamera;
	std::unique_ptr<Model> sphere;
	WorldTransform sphereWT;

public:
	void Initialize();
	void Update();
	void Draw();
};