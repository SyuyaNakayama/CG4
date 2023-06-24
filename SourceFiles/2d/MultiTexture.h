#pragma once
#include "PostEffect.h"

class MultiTexture
{
private:
	struct Vertex { Vector2 pos, uv; };

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	std::array<Vertex, 4> vertices;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> pipelineState;

	void CreateGraphicsPipelineState();
	void CreateBuffers();
public:
	void Initialize();
	// 2つのテクスチャを合成する
	void Draw(std::array<PostEffect, 2>& postEffects);
};