#pragma once
#include "BaseScene.h"
#include "FadeManager.h"
#include "SceneFactory.h"

class SceneManager final
{
private:
	BaseScene* scene_ = nullptr;
	Scene nowScene_ = Scene::Null;
	Scene nextScene_ = Scene::Null;
	FadeManager fadeManager_;
	SceneFactory* sceneFactory_ = SceneFactory::GetInstance();

	SceneManager() = default;
public:
	static SceneManager* GetInstance();
	SceneManager(const SceneManager& obj) = delete;
	void Initialize();
	void Finalize() { scene_->Finalize(); delete scene_; }
	void Update();
	void Draw();
	void SetNextScene(Scene nextScene, bool isUseFade = true);
	Scene GetNowScene() { return nowScene_; }
};