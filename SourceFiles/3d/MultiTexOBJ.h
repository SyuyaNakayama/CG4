#pragma once
#include "Model.h"

class MultiTexOBJ
{
	// Microsoft::WRL::Çè»ó™
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	std::unique_ptr<Model> model;

	static ComPtr<ID3D12PipelineState> pipelinestate;
	static ComPtr<ID3D12RootSignature> rootsignature;

	std::unique_ptr<Sprite> subTex, maskTex;

public:
	static void StaticInitialize();
	Model* GetModel() { return model.get(); }
	void SetMainTex(std::unique_ptr<Sprite> mainTex);
	void Initialize(const std::string& modelName, const std::string& subTexName, const std::string& maskTexName, bool smoothing = false);
	void Draw(const WorldTransform& worldTransform);
};