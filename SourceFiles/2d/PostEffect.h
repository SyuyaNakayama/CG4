#pragma once
#include <array>
#include "Color.h"
#include "Vector.h"
#include "DirectXCommon.h"

class PostEffect
{
private:
	struct Vertex { Vector2 pos, uv; };

	struct ConstBufferData
	{
		UINT32 effectType = 0;
		float angle = 0;
	};

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	static const float CLEAR_COLOR[4];

	static ID3D12Device* device;
	ComPtr<ID3D12Resource> texBuff;
	ComPtr<ID3D12Resource> depthBuff;
	static ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	static int staticSRVIndex;
	int srvIndex;
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	std::array<Vertex, 4> vertices;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;
	ConstBufferData* constMap = nullptr;
	static ComPtr<ID3D12RootSignature> rootSignature;
	static ComPtr<ID3D12PipelineState> pipelineState;

	void CreateBuffers();
	void CreateSRV();
	void CreateRTV();
	void CreateDSV();
public:
	static void StaticInitialize();
	void Initialize();
	void SetEffectType(UINT32 effectType) { constMap->effectType = effectType; }
	static ID3D12DescriptorHeap* GetSRV() { return descHeapSRV.Get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();
	void Draw();
	void PreDrawScene();
	void PostDrawScene();
};