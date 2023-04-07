#include "SceneManager.h"
#include "SpriteCommon.h"

SceneManager* SceneManager::GetInstance()
{
	static SceneManager sceneManager;
	return &sceneManager;
}

void SceneManager::Initialize()
{
	fadeManager_.Initialize();
}

void SceneManager::Update()
{
	fadeManager_.Update();
	if (fadeManager_.IsChange() || !scene_)
	{
		if (nextScene_ != Scene::Null)
		{
			if (scene_)
			{
				scene_->Finalize();
				delete scene_;
			}

			scene_ = sceneFactory_->CreateScene(nextScene_);
			nowScene_ = nextScene_;
			nextScene_ = Scene::Null;
			scene_->Initialize();
		}
	}

	if (!fadeManager_.IsFade()) { scene_->Update(); }
}

void SceneManager::Draw()
{
	scene_->Draw();
	if (!fadeManager_.IsFade()) { return; }
	SpriteCommon::PreDraw();
	fadeManager_.Draw();
	SpriteCommon::PostDraw();
}

void SceneManager::SetNextScene(Scene nextScene, bool isUseFade)
{
	nextScene_ = nextScene;
	if (isUseFade) { fadeManager_.FadeScene(); }
}
