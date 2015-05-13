#pragma once

#include "../../../d3d11/d3d11_render_device.h"

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
		/**
		* @struct snuffbox::D3D11Particle::ParticleProperties
		*	@brief Contains information about a particle
		* @author Daniël Konings
		*/
		struct ParticleProperties
		{
			float life_time; //!< The life time of the particle, in seconds
			XMFLOAT3 start_position; //!< The start position of the particle
			float start_size; //!< The start size of the particle
			float start_velocity_modifier; //!< The start velocity modifier of the particle
			XMFLOAT3 start_velocity; //!< The start velocity of the particle
			XMFLOAT4 start_colour; //!< The start colour of the particle
			XMFLOAT4 end_colour; //!< The end colour of the particle
			float end_size; //!< The end size of the particle
			float end_velocity_modifier; //!< The end velocity modifier
			float start_angle; //!< The start angle of this particle
			float end_angle; //!< The end angle of this particle
		};

		/// Default constructor
		D3D11Particle();

		/**
		* @brief Constructs a particle with a start time and a given description
		* @param[in] start_time (const float&) The start time
		* @param[in] pd (const snuffbox::D3D11Particle::ParticleProperties&) The description of the particle
		*/
		D3D11Particle(const float& start_time, const ParticleProperties& pd);

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
		ParticleProperties description_; //!< The properties of this particle
		float time_; //!< The current time of this particle
		XMFLOAT4 colour_; //!< The current colour of this particle
		XMFLOAT3 position_; //!< The position of this particle
		float angle_; //!< The angle of this particle
		float size_; //!< The size of this particle
		bool finished_; //!< Has this particle finished its lifetime yet?

		/**
		* @brief Lerps one value to another with a given, normalised, ratio
		* @param[in] a (const float&) The starting value
		* @param[in] b (const float&) The end value
		* @param[in] r (const float&) The ratio, this value will be clamped within [0 - 1]
		*/
		static float Lerp(const float& a, const float& b, const float& r);
		
		/**
		* @brief Lerps an XMFLOAT3 value to another with a given, normalised, ratio
		* @param[in] a (const XMFLOAT3&) The starting value
		* @param[in] b (const XMFLOAT3&) The end value
		* @param[in] r (const float&) The ratio, this value will be clamped within [0 - 1]
		*/
		static XMFLOAT3 Lerp3(const XMFLOAT3& a, const XMFLOAT3& b, const float& r);

		/**
		* @brief Lerps an XMFLOAT4 value to another with a given, normalised, ratio
		* @param[in] a (const XMFLOAT4&) The starting value
		* @param[in] b (const XMFLOAT4&) The end value
		* @param[in] r (const float&) The ratio, this value will be clamped within [0 - 1]
		*/
		static XMFLOAT4 Lerp4(const XMFLOAT4& a, const XMFLOAT4& b, const float& r);
	};
}