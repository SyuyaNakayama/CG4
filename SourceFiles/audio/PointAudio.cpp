#include "PointAudio.h"
#include "WorldTransform.h"
#include <cmath>

void PointAudio::Initialize(const std::wstring& fileName, Vector3 audioPos_, bool useCamera_, bool usePan_)
{
	Audio::Initialize(fileName);
	useCamera = useCamera_;
	audioPos = audioPos_;
	usePan = usePan_;
}

void PointAudio::Update()
{
	if (useCamera)
	{
		ViewProjection* vp = WorldTransform::GetViewProjection();
		micPos = vp->eye;
		forward = Normalize(vp->target - vp->eye);
	}
	// マイクから音源の距離
	Vector3 toMic = micPos - audioPos;
	float dis = toMic.Length();
	// 距離が1未満だと、音源の音量より大きくなるので調整
	if (dis < 1.0f) { dis = 1.0f; }
	// 距離減衰
	float dic = 20.0f * std::log10f(dis);
	if (dic <= -10000) { dic = -10000.0f; }
	SetVolume(-(long)(dic * 100.0f));
	// 音源の左右移動(パン)
	if (!usePan) { return; }
	// y軸成分を消した2成分軸で外積を取る
	Vector2 dVec[2];
	dVec[0] = { forward.x,forward.z };
	toMic.Normalize();
	dVec[1] = { toMic.x,toMic.z };
	float cross = Cross(dVec[0], dVec[1]);
	SetBalance((long)(cross * (float)panStrength));
}