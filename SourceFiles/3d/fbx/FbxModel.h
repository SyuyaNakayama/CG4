#pragma once
#include <string>
#include <vector>
#include <DirectXTex.h>
#include <windows.h>
#include <wrl.h>
#include <d3dx12.h>
#include "Matrix4.h"

struct Node
{
	std::string name;
	Vector3 scaling = { 1,1,1 },
		rotation{},
		translation{};
	Matrix4 transform, globalTransform;
	Node* parent = nullptr;
};

class FbxModel
{
public:
	struct VertexPosNormalUv
	{
		Vector3 pos, normal;
		Vector2 uv;
	};
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;
	using string = std::string;
	template <class T> using vector = std::vector<T>;

	ComPtr<ID3D12Resource> vertBuff, indexBuff, texBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;

	string name;
	vector<Node> nodes;
	Node* meshNode = nullptr;
	vector<VertexPosNormalUv> vertices;
	vector<uint16_t> indices;
	Vector3 ambient = { 1,1,1 }, diffuse = { 1,1,1 };
	TexMetadata metadata{};
	ScratchImage scratchImg{};
public:
	friend class FbxLoader;

	void CreateBuffers();
	void Draw();

	const Matrix4& GetModelTransform() { return meshNode->globalTransform; }
};