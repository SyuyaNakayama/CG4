#pragma once
#include "BaseScene.h"
#include "DebugCamera.h"
#include "MultiTexOBJ.h"

class NormalScene : public BaseScene
{
	DebugCamera debugCamera;
	MultiTexOBJ mulModel;
	std::unique_ptr<Model> model;
	WorldTransform worldTransform;
	TexProp texProps[3];
	int sceneNum = 0;

public:
	void Initialize();
	void Update();
	void Draw();
};