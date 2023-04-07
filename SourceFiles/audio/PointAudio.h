#pragma once
#include "Audio.h"
#include "ViewProjection.h"

class PointAudio : public Audio
{
private:
	Vector3 audioPos;
	Vector3 micPos;
	Vector3 forward;
	bool useCamera = true;
	bool usePan = true;
	float sPressRate = 1.0f; // 原曲との音圧の比率
	long panStrength = 2500; // パンの強さ(大きいほど左右の音量差が大きくなる)

public:
	/*
		useCameraをtrueにするとカメラの位置がマイク座標になる
		usePanをtrueにすると音源が左右移動する
	*/
	void Initialize(const std::wstring& fileName, Vector3 audioPos_ = {}, bool useCamera_ = true, bool usePan_ = true);
	void Update();
	void SetAudioPos(Vector3 pos) { audioPos = pos; }
	void SetMicPos(Vector3 pos) { micPos = pos; }
	void SetForward(Vector3 forward_) { forward = forward_; }
	void SetPanStrength(long panStrength_) { panStrength = panStrength_; }
};