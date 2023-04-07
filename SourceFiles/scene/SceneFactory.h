#pragma once
#include "AbstractSceneFactory.h"

class SceneFactory : public AbstractSceneFactory
{
private:
	SceneFactory() = default;

public:
	SceneFactory(const SceneFactory& obj) = delete;
	static SceneFactory* GetInstance();

	BaseScene* CreateScene(Scene scene) override;
};