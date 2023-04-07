#pragma once
#include <string>
#include "Quaternion.h"
#include "Color.h"

class ImGuiManager final
{
private:
	ImGuiManager() = delete;
	~ImGuiManager() = delete;

public:
	static void Initialize();
	static void Begin();
	static void End();
	static void Draw();
	static void Finalize();
	static void PrintVector(std::string str, const Vector2& vec);
	static void PrintVector(std::string str, const Vector3& vec);
	static void PrintQuaternion(std::string str, const Quaternion& q);
	static void SliderVector(std::string str, Vector2& vec, float minVal, float maxVal);
	static void SliderVector(std::string str, Vector3& vec, float minVal, float maxVal);
	static void DragVector(std::string str, Vector2& vec, float spd = 0.1f);
	static void DragVector(std::string str, Vector3& vec, float spd = 0.1f);
	static void InputVector(std::string str, Vector2& vec);
	static void InputVector(std::string str, Vector3& vec);
	static void ColorEdit(std::string str, ColorRGB& color);
	static void ColorEdit(std::string str, ColorRGBA& color);
};