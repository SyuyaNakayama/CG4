#pragma once
#include <array>
#include <wrl.h>
#include <d3d12.h>
#include "Color.h"
#include "Matrix4.h"

class PostEffect
{
private:
	struct Vertex { Vector2 pos, uv; };

	struct ConstBufferData
	{
		UINT32 effectType;
	};

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	static const float CLEAR_COLOR[4];

	ComPtr<ID3D12Resource> texBuff;
	ComPtr<ID3D12Resource> depthBuff;
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	std::array<Vertex, 4> vertices;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;
	ConstBufferData* constMap = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> pipelineState;

	void CreateGraphicsPipelineState();
	void CreateBuffers();
	void CreateSRV();
	void CreateRTV();
	void CreateDSV();
public:
	void Initialize();
	void SetEffectType(UINT32 effectType) { constMap->effectType = effectType; }
	void Draw();
	void PreDrawScene();
	void PostDrawScene();
};