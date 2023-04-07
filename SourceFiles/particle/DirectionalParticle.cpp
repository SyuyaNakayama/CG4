#include "DirectionalParticle.h"
#include "Quaternion.h"
#include "DirectXCommon.h"
#include "SpriteCommon.h"

void DirectionalParticle::Particle::Update() { position = BezierCurve(controlPoints, frame.GetRemainTimeRate()); }

void DirectionalParticle::Particle::ComputeControlPoints()
{
	// 回転軸を求める
	Vector3 axis = Normalize(end - start);
	// Quaternion生成
	Quaternion rotQ = Quaternion::MakeAxisAngle(axis, angle);
	// 制御点の追加
	controlPoints.push_back(start);
	// 制御点を線形補間で計算
	for (UINT16 i = 0; i < splitNum; i++)
	{
		// axisの分割
		Vector3 controlPoint = Lerp(start, end, (float)(i + 1) / (float)(splitNum + 1));
		// 制御点をy軸方向にradiusだけ移動させる
		controlPoint.y += radius;
		// Quaternionは原点中心に回転させるので、回転軸の始点を原点とする
		controlPoint -= start;
		// 制御点をQuaternionで回転させる
		controlPoint = Quaternion::RotateVector(controlPoint, rotQ);
		// 平行移動を相殺
		controlPoint += start;
		controlPoints.push_back(controlPoint);
	}
	controlPoints.push_back(end);
}

void DirectionalParticle::Add(const DirectionalParticle::AddProp& particleProp)
{
	particles.emplace_front();
	Particle& p = particles.front();
	p.start = particleProp.start;
	p.end = particleProp.end;
	p.scale = particleProp.scale;
	p.splitNum = particleProp.splitNum;
	p.angle = particleProp.angle;
	p.radius = particleProp.radius;
	p.frame = particleProp.lifeTime;
	p.ComputeControlPoints();
}

void DirectionalParticle::Update()
{
	particles.remove_if([](Particle& particle) { return particle.frame.CountDown(); });
	for (auto& particle : particles) { particle.Update(); }
}