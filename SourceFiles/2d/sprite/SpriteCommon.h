#pragma once
#include <array>
#include <string>
#include "DirectXCommon.h"

class SpriteCommon
{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	struct TextureData
	{
		std::string fileName;
		ComPtr<ID3D12Resource> buffer;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
		D3D12_GPU_DESCRIPTOR_HANDLE	gpuHandle{};
	};

	static const size_t MAX_SRV_COUNT = 2056;
	static const uint32_t MIP_LEVELS_DEFAULT = -1;
	// デフォルトディレクトリ
	static std::string DEFAULT_TEXTURE_DIRECTORY_PATH;
	static ComPtr<ID3D12RootSignature> rootSignature;
	static ComPtr<ID3D12PipelineState> pipelineState;
	static ComPtr<ID3D12DescriptorHeap> srvHeap;
	std::array<TextureData, MAX_SRV_COUNT> textures_;
	uint32_t textureIndex_ = 0;

	SpriteCommon() = default;
public:
	static SpriteCommon* GetInstance();
	void Initialize();
	uint32_t LoadTexture(const std::string& FILE_NAME, uint32_t mipLevels = MIP_LEVELS_DEFAULT);
	void SetTextureCommands(uint32_t index);
	static void PreDraw();
	static void PostDraw() {};
	void IncrementTextureIndex() { textureIndex_++; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT index) const { return textures_[index].gpuHandle; }
	static ID3D12DescriptorHeap* GetDescriptorHeap() { return srvHeap.Get(); }
	size_t GetIncrementSize() const;
	ID3D12Resource* GetTextureBuffer(uint32_t index) const { return textures_[index].buffer.Get(); }
};