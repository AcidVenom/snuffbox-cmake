#include "../../../d3d11/elements/particles/d3d11_particle_system.h"
#include "../../../d3d11/d3d11_camera.h"
#include "../../../application/game.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11ParticleSystem::D3D11ParticleSystem() : 
		paused_(false),
		elapsed_time_(0.0f),
		last_time_(0.0f),
		accumulated_time_(0.0f),
		max_particles_(DEFAULT_MAX_PARTICLES),
		spawn_type_(DEFAULT_SPAWN_TYPE),
		particles_per_second_(DEFAULT_PER_SECOND),
		shape_(DEFAULT_PARTICLE_SHAPE),
		blend_mode_(DEFAULT_BLEND_MODE)
	{
		set_technique("Diffuse");
		Create();
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11ParticleSystem::D3D11ParticleSystem(JS_ARGS args) :
		D3D11RenderElement(args),
		paused_(false),
		elapsed_time_(0.0f),
		last_time_(0.0f),
		accumulated_time_(0.0f),
		max_particles_(DEFAULT_MAX_PARTICLES),
		spawn_type_(DEFAULT_SPAWN_TYPE),
		particles_per_second_(DEFAULT_PER_SECOND),
		shape_(DEFAULT_PARTICLE_SHAPE),
		blend_mode_(DEFAULT_BLEND_MODE)
	{
		set_technique("Diffuse");
		Create();
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ParticleSystem::Create()
	{
		vertices_.clear();
		indices_.clear();
		particles_.clear();

		int vertex_count = max_particles_ * 4;
		int index_count = max_particles_ * 6;

		vertex_buffer_ = AllocatedMemory::Instance().Construct<D3D11VertexBuffer>(D3D11VertexBuffer::VertexBufferType::kOther);

		vertices_.resize(vertex_count);
		indices_.resize(index_count);

		int vertex = 0;
		for (unsigned int i = 0; i < indices_.size(); i += 6)
		{
			indices_.at(i)			= vertex + 0;
			indices_.at(i + 1)	= vertex + 1;
			indices_.at(i + 2)	= vertex + 3;
			indices_.at(i + 3)	= vertex + 0;
			indices_.at(i + 4)	= vertex + 3;
			indices_.at(i + 5)	= vertex + 2;

			vertices_.at(vertex).tex_coords = XMFLOAT2(0.0f, 0.0f);
			vertices_.at(vertex + 1).tex_coords = XMFLOAT2(0.0f, 1.0f);
			vertices_.at(vertex + 2).tex_coords = XMFLOAT2(1.0f, 0.0f);
			vertices_.at(vertex + 3).tex_coords = XMFLOAT2(1.0f, 1.0f);

			vertex += 4;
		}

		vertex_buffer_->Create(vertices_, indices_, false);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ParticleSystem::Update(D3D11Camera* camera, const float& dt)
	{
		if (paused_ == true)
		{
			return;
		}

		elapsed_time_ += dt;

		int vertex_offset = 0;
		int index_offset = 0;

		static const XMVECTOR top_left = XMVectorSet(-0.5f, -0.5f, 0.0f, 1.0f);
		static const XMVECTOR top_right = XMVectorSet(0.5f, -0.5f, 0.0f, 1.0f);
		static const XMVECTOR bottom_left = XMVectorSet(-0.5f, 0.5f, 0.0f, 1.0f);
		static const XMVECTOR bottom_right = XMVectorSet(0.5f, 0.5f, 0.0f, 1.0f);

		float size;
		XMVECTOR trans;

		auto billboard = [camera](const XMVECTOR& p)
		{
			XMMATRIX billboard = XMMatrixInverse(&XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), camera->view());
			XMFLOAT4X4 matrix;
			XMStoreFloat4x4(&matrix, billboard);
			matrix._14 = matrix._24 = matrix._34 = matrix._41 = matrix._42 = matrix._43 = 0;
			matrix._44 = 1;

			return XMVector4Transform(p, XMLoadFloat4x4(&matrix));
		};

		for (int i = 0; i < particles_.size(); ++i)
		{
			D3D11Particle& particle = particles_.at(i);
			particle.Update(elapsed_time_, dt);

			if (particle.finished() == true)
			{
				particles_.erase(particles_.begin() + i);
				++i;
				continue;
			}

			vertex_offset = i * VERTICES_PER_PARTICLE;
			index_offset = i * INDICES_PER_PARTICLE;

			Vertex& v1 = vertices_.at(vertex_offset);
			Vertex& v2 = vertices_.at(vertex_offset + 1);
			Vertex& v3 = vertices_.at(vertex_offset + 2);
			Vertex& v4 = vertices_.at(vertex_offset + 3);

			trans = XMLoadFloat3(&particle.position());
			size = particle.size();

			XMStoreFloat4(&v1.position, billboard(top_left * size  + trans));
			XMStoreFloat4(&v2.position, billboard(bottom_left * size + trans));
			XMStoreFloat4(&v3.position, billboard(top_right * size + trans));
			XMStoreFloat4(&v4.position, billboard(bottom_right * size + trans));

			v1.position.w = v2.position.w = v3.position.w = v4.position.w = 1.0f;
			v1.colour = v2.colour = v3.colour = v4.colour = particle.colour();
		}

		vertex_buffer_->set_num_indices(static_cast<int>(particles_.size() * 6));
		vertex_buffer_->Update(vertices_, indices_, false);

		SpawnParticles();
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11VertexBuffer* D3D11ParticleSystem::vertex_buffer()
	{
		Update(D3D11RenderDevice::Instance()->camera(), static_cast<float>(Game::Instance()->delta_time()));
		return vertex_buffer_.get();
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ParticleSystem::Play()
	{
		paused_ = false;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ParticleSystem::Pause()
	{
		paused_ = true;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ParticleSystem::Stop()
	{
		paused_ = true;
		elapsed_time_ = 0.0f;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ParticleSystem::SpawnParticles()
	{
		D3D11Particle::ParticleProperties desc = {
			2.0f,
			XMFLOAT3(static_cast<float>(rand() % 5), static_cast<float>(rand() % 5), static_cast<float>(rand() % 5)),
			10.0f,
			1.0f,
			XMFLOAT3(0.0f, -20.0f, 0.0f),
			XMFLOAT4((std::rand() % 100) / 100.0f, 0.5f, 0.75f, 1.0f),
			XMFLOAT4(1.0f, 0.5f, 0.0f, 0.0f),
			0.0f,
			0.0f,
			0.0f,
			0.0f
		};

		if (spawn_type_ == SpawnType::kInstant)
		{
			if (particles_.size() == 0)
			{
				for (unsigned int i = 0; i < static_cast<unsigned int>(max_particles_); ++i)
				{
					particles_.push_back(D3D11Particle(elapsed_time_, desc));
				}
			}
		}
		else if (spawn_type_ == SpawnType::kOvertime)
		{

		}
		else if (spawn_type_ == SpawnType::kPerSecond)
		{
			float interval = 1.0f / particles_per_second_;
			accumulated_time_ += elapsed_time_ - last_time_;

			while (accumulated_time_ > 0.0f)
			{
				if (accumulated_time_ - interval >= 0.0f)
				{
					accumulated_time_ -= interval;
					
					if (particles_.size() < max_particles_)
					{
						particles_.push_back(D3D11Particle(elapsed_time_, desc));
					}
				}
				else
				{
					break;
				}
			}
		}

		last_time_ = elapsed_time_;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11ParticleSystem::~D3D11ParticleSystem()
	{
		vertices_.clear();
		indices_.clear();
		particles_.clear();
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ParticleSystem::RegisterJS(JS_CONSTRUCTABLE obj)
	{
		D3D11RenderElement::Register(obj);
	}
}