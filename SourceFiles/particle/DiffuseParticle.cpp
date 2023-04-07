#include "DiffuseParticle.h"
#include <random>
#include "DirectXCommon.h"
#include "SpriteCommon.h"

void DiffuseParticle::Particle::Update()
{
	velocity += accel;
	position += velocity;
	float f = 1.0f / frame.GetRemainTimeRate();
	scale = s_scale + (e_scale - s_scale) / f;
}

void DiffuseParticle::Add(const AddProp& particleProp)
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_real_distribution<float> randPos(-particleProp.posRange, particleProp.posRange);
	std::uniform_real_distribution<float> randVel(-particleProp.velRange, particleProp.velRange);
	std::uniform_real_distribution<float> randAcc(-particleProp.accRange, particleProp.accRange);

	for (unsigned short i = 0; i < particleProp.addNum; i++)
	{
		particles.emplace_front();
		Particle& p = particles.front();
		p.position = Vector3(randPos(mt), randPos(mt), randPos(mt)) + particleProp.posOffset;
		p.velocity = Vector3(randVel(mt), randVel(mt), randVel(mt)) + particleProp.velOffset;
		p.accel = Vector3(randAcc(mt), randAcc(mt), 0) + particleProp.accOffset;
		p.frame = particleProp.lifeTime;
		p.scale = p.s_scale = particleProp.start_scale;
		p.e_scale = particleProp.end_scale;
	}
}

void DiffuseParticle::Update()
{
	particles.remove_if([](Particle& particle) { return particle.frame.CountDown(); });
	for (auto& particle : particles) { particle.Update(); }
}