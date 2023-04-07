#include "Skydome.h"

void Skydome::Initialize(float scale)
{
	worldTransform.Initialize();
	worldTransform.scale *= scale;
	model = Model::Create("skydome");

	Sprite* skydomeModelSprite = model->GetSprite();
	skydomeModelSprite->SetColor({ 0,0,0,1 });
}