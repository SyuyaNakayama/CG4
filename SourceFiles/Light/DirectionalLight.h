#pragma once
#include "Vector.h"
#include "Color.h"

class DirectionalLight
{
private:
	Vector3 lightdir = { 1,0,0 }; // ライト光線方向
	ColorRGB lightcolor; // ライト色
	bool active = false;

public:
	struct ConstBufferData
	{
		Vector3 lightv; // ライトへの方向を表すベクトル
		float pad;
		ColorRGB lightcolor; // ライトの色
		unsigned int active;
	};

	void SetLightDir(const Vector3& lightdir_) { lightdir = Normalize(lightdir_); }
	void SetLightColor(const ColorRGB& lightcolor_) { lightcolor = lightcolor_; }
	void SetActive(bool active_) { active = active_; }
	Vector3 GetLightDir() { return lightdir; }
	ColorRGB GetLightColor() { return lightcolor; }
	bool IsActive() { return active; }
};