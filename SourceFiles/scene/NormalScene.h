#pragma once
#include "BaseScene.h"
#include "FbxObject3d.h"
#include "DebugCamera.h"

class NormalScene : public BaseScene
{
	DebugCamera debugCamera;
	FbxModel* fbxModel_;
	FbxObject3d* fbxObject_;
	WorldTransform fbxObjWT;
	std::unique_ptr<LightGroup> lightGroup;

public:
	void Initialize();
	void Update();
	void Draw();
};