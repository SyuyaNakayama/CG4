#pragma once
#include "BaseScene.h"
#include "DebugCamera.h"
#include "MultiTexOBJ.h"

class NormalScene : public BaseScene
{
	DebugCamera debugCamera;
	MultiTexOBJ mulModel;
	WorldTransform worldTransform;
	TexProp maskTexProp;

public:
	void Initialize();
	void Update();
	void Draw();
};