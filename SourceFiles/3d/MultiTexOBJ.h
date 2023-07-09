#pragma once
#include "Model.h"

struct TexProp
{
	Vector2 tiling = { 1,1 };
	Vector2 offset;
};

class MultiTexOBJ
{
	struct ConstBufferData
	{
		TexProp texProps[3];
	};

	// Microsoft::WRL::Çè»ó™
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	std::unique_ptr<Model> model;
	static ComPtr<ID3D12PipelineState> pipelinestate;
	static ComPtr<ID3D12RootSignature> rootsignature;
	ComPtr<ID3D12Resource> constBuff;
	ConstBufferData* constMap = nullptr;
	std::unique_ptr<Sprite> subTex, maskTex;

public:
	static void StaticInitialize();
	Model* GetModel() { return model.get(); }
	void SetMainTex(std::unique_ptr<Sprite> mainTex);
	void SetTexProp(size_t index, const TexProp& texProp) { constMap->texProps[index] = texProp; }
	void Initialize(const std::string& modelName, const std::string& subTexName, const std::string& maskTexName, bool smoothing = false);
	void Draw(const WorldTransform& worldTransform);
};