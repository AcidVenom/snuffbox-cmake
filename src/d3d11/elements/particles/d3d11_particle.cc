#include "../../../d3d11/elements/particles/d3d11_particle.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11Particle::D3D11Particle() :
		finished_(false)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Particle::D3D11Particle(const float& start_time, const ParticleProperties& pd) :
		start_time_(start_time),
		description_(pd),
		colour_(pd.start_colour),
		position_(pd.start_position),
		size_(pd.start_size),
		angle_(pd.start_angle),
		finished_(false)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Particle::Update(const float& t, const float& dt)
	{
		float r = (t - start_time_) / description_.life_time;

		if (r >= 1.0f)
		{
			finished_ = true;
			return;
		}

		colour_ = Lerp4(description_.start_colour, description_.end_colour, r);
		
		XMVECTOR v = XMLoadFloat3(&description_.start_velocity);

		v *= Lerp(description_.start_velocity_modifier, description_.end_velocity_modifier, r);

		XMVECTOR p = XMLoadFloat3(&position_);
		
		p += v * dt;

		XMStoreFloat3(&position_, p);

		size_ = Lerp(description_.start_size, description_.end_size, r);
		angle_ = Lerp(description_.start_angle, description_.end_angle, r);
	}

	//---------------------------------------------------------------------------------------------------------
	const XMFLOAT4& D3D11Particle::colour() const
	{
		return colour_;
	}

	//---------------------------------------------------------------------------------------------------------
	const XMFLOAT3& D3D11Particle::position() const
	{
		return position_;
	}

	//---------------------------------------------------------------------------------------------------------
	const float& D3D11Particle::size() const
	{
		return size_;
	}

	//---------------------------------------------------------------------------------------------------------
	const bool& D3D11Particle::finished() const
	{
		return finished_;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Particle::~D3D11Particle()
	{

	}

	//---------------------------------------------------------------------------------------------------------
	float D3D11Particle::Lerp(const float& a, const float& b, const float& r)
	{
		float ratio = std::fminf(std::fmaxf(0.0f, r), 1.0f);
		return a * (1 - ratio) + ratio * b;
	}

	//---------------------------------------------------------------------------------------------------------
	XMFLOAT3 D3D11Particle::Lerp3(const XMFLOAT3& a, const XMFLOAT3& b, const float& r)
	{
		return XMFLOAT3(Lerp(a.x, b.x, r), Lerp(a.y, b.y, r), Lerp(a.z, b.z, r));
	}

	//---------------------------------------------------------------------------------------------------------
	XMFLOAT4 D3D11Particle::Lerp4(const XMFLOAT4& a, const XMFLOAT4& b, const float& r)
	{
		return XMFLOAT4(Lerp(a.x, b.x, r), Lerp(a.y, b.y, r), Lerp(a.z, b.z, r), Lerp(a.w, b.w, r));
	}
}