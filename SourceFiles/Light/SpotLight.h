#pragma once
#include "Vector.h"
#include "Color.h"
#include <cmath>

class SpotLight
{
private:
	Vector3 lightdir;
	Vector3 lightpos;
	ColorRGB lightcolor;
	Vector3 lightAtten = { 1,1,1 };
	Vector2 lightFactorAngleCos = { 0.5f,0.2f };
	bool active = false;

public:
	struct ConstBufferData
	{
		Vector3 lightv;
		float pad1;
		Vector3 lightpos;
		float pad2;
		ColorRGB lightcolor;
		float pad3;
		Vector3 lightatten;
		float pad4;
		Vector2 lightfactoranglecos;
		unsigned int active;
		float pad5;
	};

	void SetLightDir(const Vector3& lightdir_) { lightdir = Normalize(lightdir_); }
	void SetLightPos(const Vector3& lightpos_) { lightpos = lightpos_; }
	void SetLightColor(const ColorRGB& lightcolor_) { lightcolor = lightcolor_; }
	void SetLightAtten(const Vector3& lightAtten_) { lightAtten = lightAtten_; }
	void SetLightFactorAngle(const Vector2& lightFactorAngle)
	{
		lightFactorAngleCos.x = std::cos(lightFactorAngle.x);
		lightFactorAngleCos.y = std::cos(lightFactorAngle.y);
	}
	void SetActive(bool active_) { active = active_; }
	const Vector3& GetLightDir() { return lightdir; }
	const Vector3& GetLightPos() { return lightpos; }
	const ColorRGB& GetLightColor() { return lightcolor; }
	const Vector3& GetLightAtten() { return lightAtten; }
	const Vector2& GetLightFactorAngleCos() { return lightFactorAngleCos; }
	bool IsActive() { return active; }
};
