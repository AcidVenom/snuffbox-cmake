#include "../d3d11/d3d11_lighting.h"
#include "../d3d11/d3d11_light.h"
#include "../d3d11/d3d11_constant_buffer.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11Lighting::D3D11Lighting() :
		ambient_(1.0f, 1.0f, 1.0f, 1.0f)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Lighting* D3D11Lighting::Instance()
	{
		static SharedPtr<D3D11Lighting> lighting = AllocatedMemory::Instance().Construct<D3D11Lighting>();
		return lighting.get();
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Lighting::Add(D3D11Light* light)
	{
		for (D3D11Light* it : lights_)
		{
			if (it == light)
			{
				return;
			}
		}

		lights_.push_back(light);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Lighting::Remove(D3D11Light* light)
	{
		for (D3D11Light* it : lights_)
		{
			if (it == light)
			{
				it = nullptr;
			}
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Lighting::Update(D3D11ConstantBuffer* buffer)
	{
		D3D11Light* light = nullptr;
		std::vector<D3D11Light::Attributes> data;

		for (int i = static_cast<int>(lights_.size()); i >= 0; --i)
		{
			light = lights_.at(i);

			if (light == nullptr)
			{
				lights_.erase(lights_.begin() + i);
				continue;
			}

			data.push_back(light->attributes());
		}

		buffer->Map({
			ambient_,
			static_cast<int>(lights_.size()),
			&data[0]
		});
	}

	//---------------------------------------------------------------------------------------------------------
	const std::vector<D3D11Light*>& D3D11Lighting::lights() const
	{
		return lights_;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Lighting::~D3D11Lighting()
	{

	}
	
	//---------------------------------------------------------------------------------------------------------
	void D3D11Lighting::RegisterJS(JS_SINGLETON obj)
	{

	}
}