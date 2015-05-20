#include "../../../d3d11/elements/particles/d3d11_particle.h"
namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11Particle::D3D11Particle() :
		finished_(false)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Particle::D3D11Particle(const float& start_time, D3D11ParticleEffect* effect, const XMFLOAT3& position) :
		start_time_(start_time),
		effect_(effect),
		finished_(false),
		position_(position)
	{
		if (effect_ == nullptr || effect_->is_valid() == false || effect_->valid() == false)
		{
			effect_ = nullptr;
			finished_ = true;
			return;
		}

		points_ = effect_->control_points();

		for (D3D11ParticleEffect::ControlPoint& it : points_)
		{
			it.colour.Randomise();
			it.size.Randomise();
			it.velocity.Randomise();
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Particle::Update(const float& t, const float& dt)
	{
		if (effect_ == nullptr || effect_->is_valid() == false || effect_->valid() == false)
		{
			effect_ = nullptr;
			return;
		}

		const ParticleDefinition& d = effect_->definition();

		float r = (t - start_time_) / d.life_time;

		if (r >= 1.0f)
		{
			finished_ = true;
			return;
		}

		if (points_.size() < 1)
		{
			return;
		}

		D3D11ParticleEffect::ControlPoint::Result result;
		D3D11ParticleEffect::ControlPoint& first = points_.at(0);

		int current = 0;

		if (points_.size() == 1)
		{
			result = first.Interpolate(first, r);
		}
		else
		{
			while (first.ratio < r && current + 1 < points_.size())
			{
				if (points_.at(current + 1).ratio > r)
				{
					break;
				}
				first = points_.at(++current);
			}

			if (current + 1 < points_.size())
			{
				D3D11ParticleEffect::ControlPoint& second = points_.at(current + 1);
				result = first.Interpolate(second, r);
			}
			else
			{
				result = first.Interpolate(first, r);
			}
		}

		colour_ = result.colour;
		
		XMVECTOR v = XMLoadFloat3(&result.velocity);
		XMVECTOR p = XMLoadFloat3(&position_);
		
		p += v * dt;

		XMStoreFloat3(&position_, p);

		size_ = result.size;
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
}