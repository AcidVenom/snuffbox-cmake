#pragma once

#include "../../../d3d11/elements/particles/d3d11_particle.h"
#include "../../../content/content.h"

namespace snuffbox
{
	/**
	* @struct snuffbox::RangedValue
	* @brief A ranged value structure to use with the particle systems
	* @author Daniël Konings
	*/
	struct RangedValue
	{
		/**
		* @brief Construct with two given values
		* @param[in] a (const float&) The min value
		* @param[in] b (const float&) The max value
		*/
		RangedValue(const float& a, const float& b);

		float min_val; //!< The minimum value
		float max_val; //!< The maximum value

		/// Retrieves a random value from the given range
		float Get();
	};

	/**
	* @enum snuffbox::ParticleShape
	* @brief An enumerator to contain the different shapes a particle system can form
	* @author Daniël Konings
	*/
	enum ParticleShape
	{
		kSphere,
		kCone,
		kLinear
	};

	/**
	* @enum snuffbox::SpawnType
	* @brief An enumerator to contain the different spawn types a particle system can use
	* @author Daniël Konings
	*/
	enum ParticleSpawnType
	{
		kPerSecond,
		kInstant,
		kOvertime
	};

	/**
	* @struct snuffbox::ParticleDefinition
	* @brief A particle definition describing a particle effect
	* @author Daniël Konings
	*/
	struct ParticleDefinition
	{
		int max_particles; //!< The maximum number of particles
		ParticleSpawnType spawn_type; //!< The spawn type
		int particles_per_second; //!< The particles per second if the spawn type requires this
		ParticleShape shape; //!< The shape the particle system should emit in
		bool loop; //!< Should the particle system loop?
	};

	/**
	* @class snuffbox::D3D11ParticleEffect
	* @brief The actual particle effect describing a particle system and its behaviour
	* @author Daniël Konings
	*/
	class D3D11ParticleEffect : public Content
	{
	public:
		/// Default constructor
		D3D11ParticleEffect();

		/// @see snuffbox::Content
		void Load(const std::string& path);

		/**
		* @brief Creates the particle effect from a given stringified JSON
		* @param[in] str (const std::string&) The JSON string to parse
		* @param[in] path (const std::string&) The relative path
		*/
		void CreateFromJson(const std::string& str, const std::string& path);

		/**
		* @return const bool& Is this particle effect valid?
		*/
		const bool& valid() const;

		/// Default destructor
		virtual ~D3D11ParticleEffect();

		/**
		* @brief Converts a string to a spawn type
		* @param[in] str (const std::string&) The string value
		* @return snuffbox::ParticleSpawnType The converted value
		*/
		static ParticleSpawnType StringToSpawnType(const std::string& str);

		/**
		* @brief Converts a string to a spawn shape
		* @param[in] str (const std::string&) The string value
		* @return snuffbox::v The converted value
		*/
		static ParticleShape StringToShape(const std::string& str);

	private:
		ParticleDefinition definition_; //!< The definition describing this particle effect
		bool valid_; //!< Is this particle effect valid?
	};
}