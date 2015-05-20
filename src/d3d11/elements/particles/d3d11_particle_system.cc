#include "../../../d3d11/elements/particles/d3d11_particle_system.h"
#include "../../../d3d11/elements/particles/d3d11_particle_effect.h"
#include "../../../d3d11/d3d11_camera.h"
#include "../../../application/game.h"
#include "../../../content/content_manager.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11ParticleSystem::D3D11ParticleSystem() : 
		paused_(false),
		elapsed_time_(0.0f),
		last_time_(0.0f),
		accumulated_time_(0.0f),
		particle_effect_(nullptr),
    position_(0.0f, 0.0f, 0.0f)
	{
		vertex_buffer_ = AllocatedMemory::Instance().Construct<D3D11VertexBuffer>(D3D11VertexBuffer::VertexBufferType::kOther);
		set_technique("Particles");
		Create();
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11ParticleSystem::D3D11ParticleSystem(JS_ARGS args) :
		D3D11RenderElement(args),
		paused_(false),
		elapsed_time_(0.0f),
		last_time_(0.0f),
		accumulated_time_(0.0f),
		particle_effect_(nullptr),
    position_(0.0f, 0.0f, 0.0f)
	{
		JSWrapper wrapper(args);
		vertex_buffer_ = AllocatedMemory::Instance().Construct<D3D11VertexBuffer>(D3D11VertexBuffer::VertexBufferType::kOther);

		if (wrapper.Check("S") == false)
		{
			return;
		}

		set_particle_effect(wrapper.GetValue<std::string>(0, "undefined"));
		set_technique("Particles");
		Create();
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ParticleSystem::Create()
	{
		vertices_.clear();
		indices_.clear();
		particles_.clear();

		if (particle_effect_ == nullptr || particle_effect_->is_valid() == false || particle_effect_->valid() == false)
		{
			SNUFF_LOG_WARNING("Tried to create an invalid particle system");
			particle_effect_ = nullptr;
			return;
		}

		const ParticleDefinition& definition = particle_effect_->definition();
		int vertex_count = definition.max_particles * VERTICES_PER_PARTICLE;
		int index_count = definition.max_particles * INDICES_PER_PARTICLE;

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
		if (particle_effect_ == nullptr || particle_effect_->is_valid() == false || particle_effect_->valid() == false)
		{
			particle_effect_ = nullptr;
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

		auto billboard = [camera](const XMVECTOR& p, const XMVECTOR& trans, const float& size)
		{
			XMMATRIX billboard = XMMatrixInverse(&XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), camera->view());
			XMFLOAT4X4 matrix;
			XMStoreFloat4x4(&matrix, billboard);
			matrix._14 = matrix._24 = matrix._34 = matrix._41 = matrix._42 = matrix._43 = 0;
			matrix._44 = 1;

			XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f) * size;

			return XMVector4Transform(p, XMMatrixScalingFromVector(scale) * XMLoadFloat4x4(&matrix) * XMMatrixTranslationFromVector(trans));
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

			if (vertex_offset >= vertices_.size())
			{
				continue;
			}

			Vertex& v1 = vertices_.at(vertex_offset);
			Vertex& v2 = vertices_.at(vertex_offset + 1);
			Vertex& v3 = vertices_.at(vertex_offset + 2);
			Vertex& v4 = vertices_.at(vertex_offset + 3);

			trans = XMLoadFloat3(&particle.position());
			size = particle.size();

			XMStoreFloat4(&v1.position, billboard(top_left, trans, size));
			XMStoreFloat4(&v2.position, billboard(bottom_left, trans, size));
			XMStoreFloat4(&v3.position, billboard(top_right, trans, size));
			XMStoreFloat4(&v4.position, billboard(bottom_right, trans, size));

			v1.position.w = v2.position.w = v3.position.w = v4.position.w = 1.0f;
			v1.colour = v2.colour = v3.colour = v4.colour = particle.colour();
		}

		vertex_buffer_->set_num_indices(static_cast<int>(particles_.size() * 6));
		vertex_buffer_->Update(vertices_, indices_, false);

		SpawnParticles();
	}

  //---------------------------------------------------------------------------------------------------------
  const XMFLOAT3& D3D11ParticleSystem::position() const
  {
    return position_;
  }

	//---------------------------------------------------------------------------------------------------------
	D3D11VertexBuffer* D3D11ParticleSystem::vertex_buffer()
	{
		Update(D3D11RenderDevice::Instance()->camera(), static_cast<float>(Game::Instance()->delta_time()));
		return vertex_buffer_.get();
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ParticleSystem::Start()
	{
		paused_ = false;
    elapsed_time_ = 0.0f;
    last_time_ = 0.0f;
    accumulated_time_ = 0.0f;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ParticleSystem::Stop()
	{
		paused_ = true;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ParticleSystem::SpawnParticles()
	{
    if (paused_ == true)
    {
      return;
    }

		ParticleDefinition& definition = particle_effect_->definition();

		if (definition.spawn_type == ParticleSpawnType::kInstant)
		{
			if (particles_.size() == 0)
			{
				for (unsigned int i = 0; i < static_cast<unsigned int>(definition.max_particles); ++i)
				{
					definition.start_position.Randomise();
					particles_.push_back(D3D11Particle(elapsed_time_, particle_effect_, definition.start_position.value()));
				}
			}
		}
		else if (definition.spawn_type == ParticleSpawnType::kOvertime)
		{
      float interval = definition.life_time / definition.max_particles;
      accumulated_time_ += elapsed_time_ - last_time_;

      while (accumulated_time_ > 0.0f)
      {
        if (accumulated_time_ - interval >= 0.0f)
        {
          accumulated_time_ -= interval;

          if (particles_.size() < definition.max_particles)
          {
            definition.start_position.Randomise();
            XMVECTOR p = XMLoadFloat3(&definition.start_position.value());
            p += XMLoadFloat3(&position_);

            XMFLOAT3 pos;
            XMStoreFloat3(&pos, p);
            particles_.push_back(D3D11Particle(elapsed_time_, particle_effect_, pos));
          }
        }
        else
        {
          break;
        }
      }
		}
		else if (definition.spawn_type == ParticleSpawnType::kPerSecond)
		{
			float interval = 1.0f / definition.particles_per_second;
			accumulated_time_ += elapsed_time_ - last_time_;

			while (accumulated_time_ > 0.0f)
			{
				if (accumulated_time_ - interval >= 0.0f)
				{
					accumulated_time_ -= interval;
					
					if (particles_.size() < definition.max_particles)
					{
						definition.start_position.Randomise();
            XMVECTOR p = XMLoadFloat3(&definition.start_position.value());
            p += XMLoadFloat3(&position_);

            XMFLOAT3 pos;
            XMStoreFloat3(&pos, p);

						particles_.push_back(D3D11Particle(elapsed_time_, particle_effect_, pos));
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
  void D3D11ParticleSystem::set_position(const float& x, const float& y, const float& z)
  {
    position_.x = x;
    position_.y = y;
    position_.z = z;
  }

	//---------------------------------------------------------------------------------------------------------
	void D3D11ParticleSystem::set_particle_effect(const std::string& effect)
	{
		particle_effect_ = ContentManager::Instance()->Get<D3D11ParticleEffect>(effect);
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
		JSFunctionRegister funcs[] = {
			{ "setParticleEffect", JSSetParticleEffect },
      { "start", JSStart },
      { "stop", JSStop },
      { "setPosition", JSSetPosition }
		};

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ParticleSystem::JSSetParticleEffect(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11ParticleSystem* self = wrapper.GetPointer<D3D11ParticleSystem>(args.This());

		if (wrapper.Check("S") == true)
		{
			self->set_particle_effect(wrapper.GetValue<std::string>(0, "undefined"));
		}
	}

  //---------------------------------------------------------------------------------------------------------
  void D3D11ParticleSystem::JSStart(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11ParticleSystem* self = wrapper.GetPointer<D3D11ParticleSystem>(args.This());

    self->Start();
  }

  //---------------------------------------------------------------------------------------------------------
  void D3D11ParticleSystem::JSStop(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11ParticleSystem* self = wrapper.GetPointer<D3D11ParticleSystem>(args.This());

    self->Stop();
  }

  //---------------------------------------------------------------------------------------------------------
  void D3D11ParticleSystem::JSSetPosition(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11ParticleSystem* self = wrapper.GetPointer<D3D11ParticleSystem>(args.This());

    if (wrapper.Check("NN") == true)
    {
      self->set_position(
        wrapper.GetValue<float>(0, 0.0f),
        wrapper.GetValue<float>(1, 0.0f),
        wrapper.GetValue<float>(2, self->position().z)
        );
    }
  }
}