#pragma once

#include "../../../content/content.h"
#include "../../../d3d11/d3d11_render_device.h"

#include <vector>

namespace snuffbox
{
	/**
	* @struct snuffbox::RangedValue
	* @brief A ranged value structure to use with the particle systems
	* @author Daniël Konings
	*/
	struct RangedValue
	{
		/// Default constructor
		RangedValue();

		/**
		* @brief Construct with two given values
		* @param[in] a (const float&) The min value
		* @param[in] b (const float&) The max value
		*/
		RangedValue(const float& a, const float& b);

		/**
		* @brief Constructs with two given JSON values
		* @param[in] val (const v8::Handle<v8::Value>&) The JSON number for a single value or array for ranged
		*/
		RangedValue(const v8::Handle<v8::Value>& val);

		float min_val; //!< The minimum value
		float max_val; //!< The maximum value

		float value; //!< The current value of this ranged value

		/// Sets a random value from the given range
		void Randomise();
	};

	/**
	* @struct snuffbox::RangedVec3
	* @brief A vec3 of ranged values
	* @author Daniël Konings
	*/
	struct RangedVec3
	{
		/// Default constructor
		RangedVec3();

		/**
		* @brief Construct with two given values
		* @param[in] a (const XMFLOAT3&) The min value
		* @param[in] b (const XMFLOAT3&) The max value
		*/
		RangedVec3(const XMFLOAT3& a, const XMFLOAT3& b);

		/**
		* @brief Constructs with a given JSON value
		* @param[in] vec (const v8::Handle<v8::Value>&) The single value as an array or multiple as a ranged array
		*/
		RangedVec3(const v8::Handle<v8::Value>& vec);

		RangedValue x, y, z;
		
		/// Randomises the 3 values
		void Randomise();

		/**
		* @return XMFLOAT3 The current values as a float3
		*/
		XMFLOAT3 value() const;
	};

	/**
	* @struct snuffbox::RangedVec4
	* @brief A vec4 of ranged values
	* @author Daniël Konings
	*/
	struct RangedVec4
	{
		/// Default constructor
		RangedVec4();

		/**
		* @brief Construct with two given values
		* @param[in] a (const XMFLOAT4&) The min value
		* @param[in] b (const XMFLOAT4&) The max value
		*/
		RangedVec4(const XMFLOAT4& a, const XMFLOAT4& b);

		/**
		* @brief Constructs with a given JSON value
		* @param[in] vec (const v8::Handle<v8::Value>&) The single value as an array or multiple as a ranged array
		*/
		RangedVec4(const v8::Handle<v8::Value>& vec);

		RangedValue x, y, z, w;

		/// Randomises the 4 values
		void Randomise();

		/**
		* @return XMFLOAT4 The current values as a float4
		*/
		XMFLOAT4 value() const;
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
		float life_time; //!< The life time of each particle
		int max_particles; //!< The maximum number of particles
		ParticleSpawnType spawn_type; //!< The spawn type
		int particles_per_second; //!< The particles per second if the spawn type requires this
		bool loop; //!< Should the particle system loop?
		int loop_length; //!< The loop length of this particle system
		RangedVec3 start_position; //!< The start position of each particle relative to the particle systems translation
		RangedValue start_angle; //!< The start angle of each particle
	};

	/**
	* @class snuffbox::D3D11ParticleEffect
	* @brief The actual particle effect describing a particle system and its behaviour
	* @author Daniël Konings
	*/
	class D3D11ParticleEffect : public Content
	{
	public:

		/**
		* @struct snuffbox::D3D11ParticleEffect::ControlPoint
		* @brief A control point to use with the interpolation of particle systems
		* @author Daniël Konings
		*/
		struct ControlPoint
		{
			/**
			* @enum snuffbox::D3D11ParticleEffect::ControlPoint::IsSet
			* @brief Used to check if values are set for this control point
			* @author Daniël Konings
			*/
			enum IsSet : int
			{
				kRatio,
				kVelocity,
				kColour,
				kSize,
				kAngularVelocity
			};

			/**
			* @brief Constructs a control point from a JSON value
			* @param[in] val (const v8::Handle<v8::Value>&) The input value to convert
			* @param[in] path (const std::string&) The relative path for error logging
			* @param[in] idx (const int&) The index the control point is located at in the JSON
			*/
			ControlPoint(const v8::Handle<v8::Value>& val, const std::string& path, const int& idx);

			float ratio; //!< The ratio this control point is located at
			RangedVec3 velocity; //!< The velocity at this control point
			RangedVec4 colour; //!< The colour at this control point
			RangedValue size; //!< The size at this control point
			RangedValue angular_velocity; //!< The angular velocity at this control point
			bool is_set[5]; //!< A list of booleans to check if a value is set

			/**
			* @struct snuffbox::D3D11ParticleEffect::ControlPoint::Result
			* @brief An interpolation result of one control point to another
			* @author Daniël Konings
			*/
			struct Result
			{
				XMFLOAT3 velocity; //!< The resulting velocity
				XMFLOAT4 colour; //!< The resulting colour
				float size; //!< The resulting size
				float angular_velocity; //!< The resulting angular velocity
			};

			/**
			* @brief Interpolates this control point to another
			* @param[in] other (snuffbox::D3D11ParticleEffect::ControlPoint&) The control point to interpolate to
			* @param[in] current (const float&) The current, absolute, ratio
			* @return snuffbox::D3D11ParticleEffect::ControlPoint::Result The resulting data
			*/
			Result Interpolate(ControlPoint& other, const float& current);

			/**
			* @struct snuffbox::D3D11ParticleEffect::ControlPoint::Sorter
			* @brief Sorts control points by ratio
			* @author Daniël Konings
			*/
			struct Sorter
			{
				bool operator()(ControlPoint& a, ControlPoint& b);
			};
		};

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

		/**
		* @return D3D11ParticleEffect::ParticleDefinition& The definition of this particle effect
		*/
		ParticleDefinition& definition();

		/**
		* @return std::vector<snuffbox::D3D11ParticleEffect::ControlPoint>&) The vector of control points
		*/
		std::vector<ControlPoint>& control_points();

		/// Default destructor
		virtual ~D3D11ParticleEffect();

		/**
		* @brief Converts a string to a spawn type
		* @param[in] str (const std::string&) The string value
		* @return snuffbox::ParticleSpawnType The converted value
		*/
		static ParticleSpawnType StringToSpawnType(const std::string& str);

	private:
		ParticleDefinition definition_; //!< The definition describing this particle effect
		bool valid_; //!< Is this particle effect valid?
		std::vector<ControlPoint> control_points_; //!< The control points of this particle effect

	public:
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