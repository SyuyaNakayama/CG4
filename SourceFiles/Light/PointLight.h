#pragma once
#include "Vector.h"
#include "Color.h"

class PointLight
{
private:
	Vector3 lightpos;
	ColorRGB lightcolor;
	Vector3 lightAtten = { 1,1,1 }; // ÉâÉCÉgãóó£å∏êäåWêî
	bool active = false;

public:
	struct ConstBufferData
	{
		Vector3 lightpos;
		float pad1;
		ColorRGB lightcolor;
		float pad2;
		Vector3 lightAtten;
		unsigned int active;
	};

	void SetLightPos(const Vector3& lightpos_) { lightpos = lightpos_; }
	void SetLightColor(const ColorRGB& lightcolor_) { lightcolor = lightcolor_; }
	void SetLightAtten(const Vector3& lightAtten_) { lightAtten = lightAtten_; }
	void SetActive(bool active_) { active = active_; }
	const Vector3& GetLightPos() { return lightpos; }
	const ColorRGB& GetLightColor() { return lightcolor; }
	const Vector3& GetLightAtten() { return lightAtten; }
	bool IsActive() { return active; }
};