#pragma once

#include "../../../d3d11/elements/particles/d3d11_particle.h"
#include "../../../d3d11/elements/d3d11_render_element.h"
#include "../../../d3d11/d3d11_vertex_buffer.h"

#include <queue>

namespace snuffbox
{
	class D3D11ParticleEffect;

	/**
	* @class snuffbox::D3D11ParticleSystem
	* @brief A particle system that updates and renders particles relative to itself
	* @author Daniël Konings
	*/
	class D3D11ParticleSystem : public D3D11RenderElement, public JSObject
	{
	public:

		/// Default constructor
		D3D11ParticleSystem();

		/// JavaScript constructor
		D3D11ParticleSystem(JS_ARGS args);

		/// Creates the vertex buffer
		void Create();

		/**
		* @brief Updates the particle system
		* @param[in] camera (snuffbox::D3D11Camera*) The camera to billboard the particles to
		* @param[in] dt (const float&) The current delta time
		*/
		void Update(D3D11Camera* camera, const float& dt);

		/// Starts the particle system
		void Start();

		/// Stops the particle system
		void Stop();

		/**
		* @brief Spawns particles adjusted to the translation of the emitter
		* @param[in] trans (const XMVECTOR&) The translation
		*/
		void SpawnParticles(const XMVECTOR& trans);

		/**
		* @brief Spawns a single particle
		* @param[in] definition (snuffbox::ParticleDefinition&) The definition
		* @param[in] trans (const XMVECTOR&) The translation of the emitter
		*/
		void SpawnParticle(ParticleDefinition& definition, const XMVECTOR& trans);

		/**
		* @brief Sets the effect of this particle system
		* @param[in] effect (const std::string&) The relative path to the effect to set
		*/
		void set_particle_effect(const std::string& effect);

		/// @see snuffbox::D3D11RenderElement
		D3D11VertexBuffer* vertex_buffer();

		/// Default destructor
		virtual ~D3D11ParticleSystem();

	private:
		float last_time_; //!< The last time the spawner was ran
		float elapsed_time_; //!< The elapsed time of this particle system
		float accumulated_time_; //!< The total accumulated time
		std::deque<D3D11Particle> particles_; //!< The particles created by this emitter
		SharedPtr<D3D11VertexBuffer> vertex_buffer_; //!< The vertex buffer of all particles together
		bool paused_; //!< Is this particle system paused?
		std::vector<Vertex> vertices_; //!< The vertices of this particle system
		std::vector<int> indices_; //!< The indices of this particle system
		int num_particles_; //!< The current number of particles
		int old_max_; //!< The old maximum number of particles

		static const int VERTICES_PER_PARTICLE = 4; //!< The number of vertices a particle uses
		static const int INDICES_PER_PARTICLE = 6; //!< The number of indices a particle uses

		D3D11ParticleEffect* particle_effect_; //!< The particle effect definition of this system

	public:
		JS_NAME("ParticleSystem");
		static void RegisterJS(JS_CONSTRUCTABLE obj);
    static void JSSetParticleEffect(JS_ARGS args);
    static void JSStart(JS_ARGS args);
    static void JSStop(JS_ARGS args);
	};
}