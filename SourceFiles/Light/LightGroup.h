#pragma once
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "CircleShadow.h"
#include <wrl.h>
#include <d3d12.h>
#include <memory>

class LightGroup
{
public:
	static const int DIR_LIGHT_NUM = 3;
	static const int POINT_LIGHT_NUM = 8;
	static const int SPOT_LIGHT_NUM = 3;
	static const int CIRCLE_SHADOW_NUM = 1;

private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	struct ConstBufferData
	{
		ColorRGB ambientColor;
		float pad1;
		DirectionalLight::ConstBufferData dirLights[DIR_LIGHT_NUM];
		PointLight::ConstBufferData pointLights[POINT_LIGHT_NUM];
		SpotLight::ConstBufferData spotLights[SPOT_LIGHT_NUM];
		CircleShadow::ConstBufferData circleShadows[CIRCLE_SHADOW_NUM];
	};

	ComPtr<ID3D12Resource> constBuff;
	ColorRGB ambientColor;
	DirectionalLight dirLights[DIR_LIGHT_NUM];
	PointLight pointLights[POINT_LIGHT_NUM];
	SpotLight spotLights[SPOT_LIGHT_NUM];
	CircleShadow circleShadows[CIRCLE_SHADOW_NUM];
	bool dirty = false;
	ConstBufferData* constMap = nullptr;

	void TransferConstBuffer();
public:
	static std::unique_ptr<LightGroup> Create();
	void Initialize();
	void Update();
	void Draw();
	void DefaultLightSetting();
	void SetAmbientColor(const ColorRGB& color) { ambientColor = color; dirty = true; }
#pragma region 平行光源Setter
	void SetDirLightActive(size_t index, bool active);
	void SetDirLightDir(size_t index, const Vector3& lightDir);
	void SetDirLightColor(size_t index, const ColorRGB& lightcolor);
#pragma endregion
#pragma region 点光源Setter
	void SetPointLightActive(size_t index, bool active);
	void SetPointLightPos(size_t index, const Vector3& lightpos);
	void SetPointLightColor(size_t index, const ColorRGB& lightcolor);
	void SetPointLightAtten(size_t index, const Vector3& lightAtten);
#pragma endregion
#pragma region スポットライトSetter
	void SetSpotLightActive(size_t index, bool active);
	void SetSpotLightDir(size_t index, const Vector3& lightdir);
	void SetSpotLightPos(size_t index, const Vector3& lightpos);
	void SetSpotLightColor(size_t index, const ColorRGB& lightcolor);
	void SetSpotLightAtten(size_t index, const Vector3& lightAtten);
	void SetSpotLightFactorAngle(size_t index, const Vector2& lightFactorAngle);
#pragma endregion
#pragma region 丸影Setter
	void SetCircleShadowActive(size_t index, bool active);
	void SetCircleShadowCasterPos(size_t index, const Vector3& casterPos);
	void SetCircleShadowDir(size_t index, const Vector3& lightdir);
	void SetCircleShadowDistanceCasterLight(size_t index, float distanceCasterLight);
	void SetCircleShadowAtten(size_t index, const Vector3& lightAtten);
	void SetCircleShadowFactorAngle(size_t index, const Vector2& lightFactorAngle);
#pragma endregion
	bool GetPointLightActive(size_t index) { return pointLights[index].IsActive(); }
};