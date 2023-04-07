#pragma once
#include <array>
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <memory>
#include "Matrix4.h"
#include "Color.h"

class Sprite
{
public:
	enum class VertexNumber
	{
		LB, // 左下
		LT, // 左上
		RB, // 右下
		RT  // 右上
	};

private:
	struct Vertex { Vector2 pos, uv; };

	struct ConstBufferData
	{
		Matrix4 mat;
		ColorRGBA color;
	};

	const static Matrix4 matProj;
	std::array<Vertex, 4> vertices;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;
	ConstBufferData* constMap = nullptr;
	Vertex* vertMap = nullptr;

	Matrix4 matWorld;
	Vector2 position_;
	float rotation_ = 0;
	ColorRGBA color_;
	Vector2 size_; // 表示サイズ
	Vector2 anchorPoint_;
	bool isFlipX_ = false;
	bool isFlipY_ = false;
	bool isInvisible_ = false;
	uint32_t textureIndex_ = 0;
	Vector2 textureLeftTop_; // 切り取り領域の左上座標
	Vector2 textureSize_; // 切り取り領域のサイズ
	bool dirty = true;

	void AdjustTextureSize();
public:
	void Initialize(uint32_t textureIndex);
	static std::unique_ptr<Sprite> Create(const std::string& FILE_NAME);
	void Update();
	void Draw();

	// setter
	void SetPosition(const Vector2& position) { position_ = position; dirty = true; }
	void SetRotation(float rotation) { rotation_ = rotation; dirty = true; }
	void SetColor(const ColorRGBA& color) { color_ = color; dirty = true; }
	void SetSize(const Vector2& size) { size_ = size; dirty = true; }
	void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; dirty = true; }
	void SetIsFlipX(bool isFlipX = true) { isFlipX_ = isFlipX; dirty = true; }
	void SetIsFlipY(bool isFlipY = true) { isFlipY_ = isFlipY; dirty = true; }
	void SetIsInvisible(bool isInvisible = true) { isInvisible_ = isInvisible; }
	void SetTextureLeftTop(const Vector2& textureLeftTop) { textureLeftTop_ = textureLeftTop; dirty = true; }
	void SetTextureSize(const Vector2& textureSize) { textureSize_ = textureSize; dirty = true; }
	// getter
	const Vector2& GetPosition()					const { return position_; }
	float GetRotation()								const { return rotation_; }
	const ColorRGBA& GetColor()						const { return color_; }
	const Vector2& GetSize()						const { return size_; }
	const Vector2& GetAnchorPoint()					const { return anchorPoint_; }
	bool GetIsFlipX()								const { return isFlipX_; }
	bool GetIsFlipY()								const { return isFlipY_; }
	bool GetIsInvisible()							const { return isInvisible_; }
	uint32_t GetTextureIndex()						const { return textureIndex_; }
	const Vector2& GetTextureLeftTop()				const { return textureLeftTop_; }
	const Vector2& GetTextureSize()					const { return textureSize_; }
	const Vector2& GetVerticesUv(VertexNumber vNum) const { return vertices[(size_t)vNum].uv; }
};