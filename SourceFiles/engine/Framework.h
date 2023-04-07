#pragma once
#include "SceneManager.h"
#include "DirectXCommon.h"

class Framework
{
protected:
	WindowsAPI* wAPI = WindowsAPI::GetInstance();
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Input* input = Input::GetInstance();
	SceneManager* sceneManager = SceneManager::GetInstance();

public:
	virtual ~Framework() = default;
	virtual void Initialize();
	virtual void Finalize();
	virtual void Update();
	virtual void Draw() = 0;
	virtual bool IsEndRequest();
	void Run();
};