#pragma once

#include "../../../d3d11/d3d11_render_device.h"
#include "../../../d3d11/elements/particles/d3d11_particle_effect.h"

namespace snuffbox
{
	/**
	* @class snuffbox::D3D11Particle
	*	@brief A particle used within a particle system
	* @author Daniël Konings
	*/
	class D3D11Particle
	{
	public:
		/// Default constructor
		D3D11Particle();

		/**
		* @brief Constructs a particle with a start time and a given description
		* @param[in] start_time (const float&) The start time
		* @param[in] effect (snuffbox::D3D11ParticleEffect*) The definition of the particle system
		* @param[in] position (const XMFLOAT3&) The start position of the particle
		*/
		D3D11Particle(const float& start_time, D3D11ParticleEffect* effect, const XMFLOAT3& position);

		/**
		* @brief Updates and interpolates this particle with a given time
		* @param[in] t (const float&) The current time
		* @param[in] dt (const float&) The current delta time
		*/
		void Update(const float& t, const float& dt);

		/**
		* @return const XMFLOAT4& The current colour of this particle
		*/
		const XMFLOAT4& colour() const;

		/**
		* @return const XMFLOAT3& The position of this particle
		*/
		const XMFLOAT3& position() const;

		/**
		* @return const float& The size of this particle
		*/
		const float& size() const;

		/**
		* @return const float& The rotation of this particle
		*/
		const float& angle() const;

		/**
		* @return const bool& Has this particle finished its lifetime yet?
		*/
		const bool& finished() const;

		/// Default destructor
		~D3D11Particle();

	private:
		float start_time_; //!< The start time of this particle
		D3D11ParticleEffect* effect_; //!< The particle system definition of this particle
		float time_; //!< The current time of this particle
		XMFLOAT4 colour_; //!< The current colour of this particle
		XMFLOAT3 position_; //!< The position of this particle
		float size_; //!< The size of this particle
		bool finished_; //!< Has this particle finished its lifetime yet?
		std::vector<D3D11ParticleEffect::ControlPoint> points_; //!< The copied control points
	};
}