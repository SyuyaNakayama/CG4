#pragma once
#include "Framework.h"
#include "MultiTexture.h"
#include "ViewProjection.h"

class MyGame : public Framework
{
private:
	int mode = 0; 
	std::array<PostEffect, 5> postEffects;
	std::array<PostEffect, 3> tempTextures; // �ꎞ�I�ȃe�N�X�`����ۑ����Ƃ��̈�
	MultiTexture multiTextures;
	ViewProjection viewProjection;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
};