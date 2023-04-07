#include "Sprite.h"
#include "SpriteCommon.h"
#include "D3D12Common.h"

// 平行投影行列
Matrix4 OrthoGraphic(const Vector2& windowSize)
{
	Matrix4 matProj;
	// 平行投影行列の生成
	matProj.m[0][0] = 2.0f / windowSize.x;
	matProj.m[1][1] = -2.0f / windowSize.y;
	matProj.m[3][0] = -1.0f;
	matProj.m[3][1] = 1.0f;
	return matProj;
}

const Matrix4 Sprite::matProj = OrthoGraphic(WindowsAPI::WIN_SIZE);

void Sprite::Initialize(uint32_t textureIndex)
{
	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
	ID3D12Resource* vertBuff = nullptr;
	CreateBuffer<Vertex>(&vertBuff, &vertMap, sizeVB);

	// 全頂点に対して座標をコピー
	copy(vertices.begin(), vertices.end(), vertMap);

	// GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(Vertex);

	// 定数バッファ
	CreateBuffer(constBuff.GetAddressOf(),
		&constMap, (sizeof(ConstBufferData) + 0xff) & ~0xff);

	textureIndex_ = textureIndex;
	AdjustTextureSize();
	size_ = textureSize_;
}

std::unique_ptr<Sprite> Sprite::Create(const std::string& FILE_NAME)
{
	std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
	sprite->Initialize(SpriteCommon::GetInstance()->LoadTexture(FILE_NAME));
	return sprite;
}

void Sprite::AdjustTextureSize()
{
	ID3D12Resource* textureBuffer = SpriteCommon::GetInstance()->GetTextureBuffer(textureIndex_);
	assert(textureBuffer);

	D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

	textureSize_.x = static_cast<float>(resDesc.Width);
	textureSize_.y = static_cast<float>(resDesc.Height);
}

void Sprite::Update()
{
	if (isInvisible_) { return; }
	if (!dirty) { return; }
	dirty = false;

	float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;

	if (isFlipX_) { left = -left; right = -right; }
	if (isFlipY_) { top = -top; bottom = -bottom; }

	vertices[(size_t)VertexNumber::LB].pos = { left, bottom };
	vertices[(size_t)VertexNumber::LT].pos = { left, top };
	vertices[(size_t)VertexNumber::RB].pos = { right, bottom };
	vertices[(size_t)VertexNumber::RT].pos = { right, top };

	ID3D12Resource* textureBuffer = SpriteCommon::GetInstance()->GetTextureBuffer(textureIndex_);
	D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

	float tex_left = textureLeftTop_.x / resDesc.Width;
	float tex_right = (textureLeftTop_.x + textureSize_.x) / resDesc.Width;
	float tex_top = textureLeftTop_.y / resDesc.Height;
	float tex_bottom = (textureLeftTop_.y + textureSize_.y) / resDesc.Height;

	vertices[(size_t)VertexNumber::LB].uv = { tex_left, tex_bottom };
	vertices[(size_t)VertexNumber::LT].uv = { tex_left, tex_top };
	vertices[(size_t)VertexNumber::RB].uv = { tex_right, tex_bottom };
	vertices[(size_t)VertexNumber::RT].uv = { tex_right, tex_top };

	Matrix4 matRot, matTrans;
	matRot = Matrix4::RotateZ(rotation_);
	matTrans = Matrix4::Translate(position_);

	matWorld = matRot * matTrans;

	// GPU転送
	constMap->mat = matWorld * matProj;
	constMap->color = color_;
	std::copy(std::begin(vertices), std::end(vertices), vertMap);
}

void Sprite::Draw()
{
	if (isInvisible_) { return; }

	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	SpriteCommon* spriteCommon = SpriteCommon::GetInstance();

	spriteCommon->SetTextureCommands(textureIndex_);

	// 頂点バッファビューの設定コマンド
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	// 描画コマンド
	cmdList->DrawInstanced((UINT)vertices.size(), 1, 0, 0); // 全ての頂点を使って描画
}