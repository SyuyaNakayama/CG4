#pragma once
#include "Framework.h"
#include "MultiTexture.h"
#include "ViewProjection.h"

class MyGame : public Framework
{
private:
	int mode = 0; 
	ViewProjection viewProjection;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
};