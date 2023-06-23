#pragma once
#include <array>
#include <wrl.h>
#include <d3d12.h>
#include "Color.h"
#include "Matrix4.h"
#include "PostEffect.h"

class MultiTexture
{
private:
	struct Vertex { Vector2 pos, uv; };

	struct ConstBufferData
	{
		UINT32 effectType;
		UINT32 index;
	};

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	static const float CLEAR_COLOR[4];

	ComPtr<ID3D12Resource> texBuff[2];
	ComPtr<ID3D12Resource> depthBuff;
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	std::array<Vertex, 4> vertices;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	ConstBufferData* constMap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> pipelineState;

	void CreateGraphicsPipelineState();
	void CreateBuffers();
	void CreateSRV();
	void CreateRTV();
	void CreateDSV();
public:
	void SetConstBufferData(const ConstBufferData& constBufferData) { *constMap = constBufferData; }
	void Initialize();
	void Draw(std::array<PostEffect, 2>& postEffects);
	void PreDrawScene();
	void PostDrawScene();
};