#include "FbxModel.h"
#include "D3D12Common.h"
#include "SpriteCommon.h"
using namespace DirectX;

const std::string FbxModel::BASE_DIRECTORY = "Resources/fbx/";
const std::string FbxModel::DEFAULT_TEXTURE_FILE_NAME = "white1x1.png";

Vector3 FbxDouble3ToVector3(const FbxDouble3& num1)
{
	return Vector3((float)num1[0], (float)num1[1], (float)num1[2]);
}
Vector3 FbxDouble4ToVector3(const FbxDouble4& num1)
{
	return Vector3((float)num1[0], (float)num1[1], (float)num1[2]);
}

void FbxModel::ParseNodeRecursive(FbxNode* fbxNode, Node* parent)
{
	nodes.emplace_back();
	Node& node = nodes.back();
	node.name = fbxNode->GetName();

	FbxDouble3 rotation = fbxNode->LclRotation.Get(),
		scaling = fbxNode->LclScaling.Get(),
		translation = fbxNode->LclTranslation.Get();

	node.rotation = FbxDouble3ToVector3(rotation) * PI / 180.0f;
	node.scaling = FbxDouble3ToVector3(scaling);
	node.translation = FbxDouble3ToVector3(translation);

	Matrix4 matScaling = Matrix4::Scale(node.scaling),
		matRotation = Matrix4::Rotate(node.rotation),
		matTranslation = Matrix4::Translate(node.translation);

	node.globalTransform = node.transform = matScaling * matRotation * matTranslation;

	if (parent)
	{
		node.parent = parent;
		node.globalTransform *= parent->globalTransform;
	}

	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

	if (fbxNodeAttribute)
	{
		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			meshNode = &node;
			ParseMesh(fbxNode);
		}
	}

	for (size_t i = 0; i < fbxNode->GetChildCount(); i++)
	{
		ParseNodeRecursive(fbxNode->GetChild(i), &node);
	}
}

void FbxModel::ParseMesh(FbxNode* fbxNode)
{
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	ParseMeshVertices(fbxMesh);
	ParseMeshFaces(fbxMesh);
	ParseMaterial(fbxNode);
}

std::string FbxModel::ExtractFileName(const string& PATH)
{
	size_t pos1 = PATH.rfind('\\');
	if (pos1 != string::npos) { return PATH.substr(pos1 + 1, PATH.size() - pos1 - 1); }

	pos1 = PATH.rfind('/');
	if (pos1 != string::npos) { return PATH.substr(pos1 + 1, PATH.size() - pos1 - 1); }

	return PATH;
}

void FbxModel::ParseMeshVertices(FbxMesh* fbxMesh)
{
	const int CONTROL_POINTS_COUNT = fbxMesh->GetControlPointsCount();

	FbxModel::VertexPosNormalUv vert{};
	vertices.resize(CONTROL_POINTS_COUNT, vert);

	FbxVector4* pCoord = fbxMesh->GetControlPoints();

	for (size_t i = 0; i < CONTROL_POINTS_COUNT; i++)
	{
		FbxModel::VertexPosNormalUv& vertex = vertices[i];
		vertex.pos = FbxDouble4ToVector3(pCoord[i]);
	}
}

void FbxModel::ParseMeshFaces(FbxMesh* fbxMesh)
{
	assert(indices.size() == 0);

	const int POLYGON_COUNT = fbxMesh->GetPolygonCount();
	const int TEXTURE_UV_COUNT = fbxMesh->GetTextureUVCount();

	FbxStringList uvNames;
	fbxMesh->GetUVSetNames(uvNames);

	for (size_t i = 0; i < POLYGON_COUNT; i++)
	{
		const int POLYGON_SIZE = fbxMesh->GetPolygonSize(i);
		assert(POLYGON_SIZE <= 4);

		for (size_t j = 0; j < POLYGON_SIZE; j++)
		{
			int index = fbxMesh->GetPolygonVertex(i, j);
			assert(index >= 0);

			FbxModel::VertexPosNormalUv& vertex = vertices[index];
			FbxVector4 normal;
			if (fbxMesh->GetPolygonVertexNormal(i, j, normal))
			{
				vertex.normal = FbxDouble4ToVector3(normal);
			}

			if (TEXTURE_UV_COUNT > 0)
			{
				FbxVector2 uvs;
				bool lUnmappedUV;

				if (fbxMesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, lUnmappedUV))
				{
					vertex.uv.x = (float)uvs[0];
					vertex.uv.y = (float)uvs[1];
				}
			}

			if (j < 3) { indices.push_back(index); }
			else
			{
				int index2 = indices[indices.size() - 1];
				int index3 = index;
				int index0 = indices[indices.size() - 3];
				indices.push_back(index2);
				indices.push_back(index3);
				indices.push_back(index0);
			}
		}
	}
}

void FbxModel::ParseMaterial(FbxNode* fbxNode)
{
	const int MATERIAL_COUNT = fbxNode->GetMaterialCount();
	if (MATERIAL_COUNT > 0)
	{
		FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);
		bool textureLoaded = false;

		if (material)
		{
			if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
			{
				FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(material);
				ambient = FbxDouble3ToVector3(lambert->Ambient);
				diffuse = FbxDouble3ToVector3(lambert->Diffuse);
			}

			const FbxProperty DIFFUSE_PROPERTY = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (DIFFUSE_PROPERTY.IsValid())
			{
				const FbxFileTexture* TEXTURE = DIFFUSE_PROPERTY.GetSrcObject<FbxFileTexture>();
				if (TEXTURE)
				{
					const char* FILEPATH = TEXTURE->GetFileName();
					string path_str(FILEPATH);
					string name = ExtractFileName(path_str);
					LoadTexture(BASE_DIRECTORY + this->name + "/" + name);
					textureLoaded = true;
				}
			}
		}

		if (!textureLoaded) { LoadTexture(BASE_DIRECTORY + DEFAULT_TEXTURE_FILE_NAME); }
	}
}

void FbxModel::LoadTexture(const string& FULLPATH)
{
	HRESULT result = S_FALSE;

	wchar_t wfilepath[128];
	MultiByteToWideChar(CP_ACP, 0, FULLPATH.c_str(), -1, wfilepath, _countof(wfilepath));
	result = LoadFromWICFile(wfilepath, WIC_FLAGS_NONE, &metadata, scratchImg);
	assert(SUCCEEDED(result));
}

void FbxModel::CreateBuffers()
{
#pragma region 頂点バッファ設定
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * vertices.size());
	CreateBuffer(&vertBuff, &vertMap, sizeVB);
	std::copy(vertices.begin(), vertices.end(), vertMap);

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);
#pragma endregion
#pragma region インデックスバッファ設定
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices.size());
	CreateBuffer(&indexBuff, &indexMap, sizeIB);
	std::copy(indices.begin(), indices.end(), indexMap);

	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;
#pragma endregion
#pragma region テクスチャバッファ設定
	Result result;
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);
	assert(img);

	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format, metadata.width, (UINT)metadata.height,
		(UINT16)metadata.arraySize, (UINT16)metadata.mipLevels);

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE, &texresDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&texBuff));

	result = texBuff->WriteToSubresource(0, nullptr, img->pixels, (UINT)img->rowPitch, (UINT)img->slicePitch);
#pragma endregion
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = texBuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(texBuff.Get(), &srvDesc,
		SpriteCommon::GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
}

void FbxModel::Draw()
{
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	ID3D12DescriptorHeap* descHeap = SpriteCommon::GetDescriptorHeap();
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	cmdList->SetGraphicsRootDescriptorTable(1, descHeap->GetGPUDescriptorHandleForHeapStart());
	cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}