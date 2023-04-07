#pragma once
#include "Model.h"

class Skydome
{
private:
	WorldTransform worldTransform;
	std::unique_ptr<Model> model;

public:
	void Initialize(float scale);
	void Update() { worldTransform.Update(); model->Update(); }
	void Draw() { model->Draw(worldTransform); }
};