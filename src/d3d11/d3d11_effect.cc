#include "../d3d11/d3d11_effect.h"
#include "../d3d11/d3d11_blend_state.h"
#include "../d3d11/d3d11_depth_state.h"
#include "../d3d11/d3d11_shader.h"

#include "../content/content_manager.h"
#include "../platform/platform_text_file.h"

#include "../application/game.h"

using namespace v8;

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11Effect::D3D11Effect() :
		Content(ContentTypes::kEffect)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Effect::Load(const std::string& path)
	{
		techniques_.clear();

		std::string full_path = Game::Instance()->path() + "/" + path;
		TextFile file;
		bool valid = file.Open(full_path);

		if (valid == false)
		{
			SNUFF_LOG_ERROR("Could not open effect file '" + path + "'");
			return;
		}

		JSStateWrapper* wrapper = JSStateWrapper::Instance();
		Isolate* isolate = wrapper->isolate();
		HandleScope scope(wrapper->isolate());

		TryCatch try_catch;
		Local<Value> json = JSON::Parse(String::NewFromUtf8(isolate, file.Read().c_str()));

		if (json.IsEmpty())
		{
			std::string buffer;
			JSStateWrapper::Instance()->GetException(&try_catch, &buffer);

			SNUFF_LOG_ERROR(buffer);
			SNUFF_LOG_ERROR("Invalid JSON for effect '" + path + "'");
			return;
		}

		if (json->IsObject() == false)
		{
			SNUFF_LOG_ERROR("Input JSON is not of an object type for an effect '" + path + "'");
			return;
		}

		Local<Object> obj = json->ToObject();

		Local<Value> tech = obj->Get(String::NewFromUtf8(isolate, "techniques"));

		if (tech.IsEmpty() || tech->IsObject() == false)
		{
			SNUFF_LOG_ERROR("'techniques' of '" + path + "' are not of an object type or undefined");
			return;
		}

		Local<Array> techniques = tech->ToObject()->GetPropertyNames();

		std::string name;
		for (unsigned int i = 0; i < techniques->Length(); ++i)
		{
			name = *String::Utf8Value(techniques->Get(i)->ToString());
			Local<Value> passes = tech->ToObject()->Get(techniques->Get(i))->ToObject()->Get(String::NewFromUtf8(isolate, "passes"));

			if (passes.IsEmpty() || passes->IsArray() == false)
			{
				SNUFF_LOG_ERROR("'passes' of technique '" + name + "' of '" + path + "' is not of an array type or is undefined");
				continue;
			}

			std::vector<Pass> new_passes;

			Local<Array> p = passes->ToObject()->GetPropertyNames();
			for (unsigned int j = 0; j < p->Length(); ++j)
			{
				Local<Value> pass = passes->ToObject()->Get(j);

				if (pass.IsEmpty() || pass->IsObject() == false)
				{
					SNUFF_LOG_ERROR("Pass " + std::to_string(j + 1) + " of technique '" + name + "' of '" + path + "' is not of an object type or is undefined");
					continue;
				}

				Pass new_pass;
				new_pass.shader = ContentManager::Instance()->Get<D3D11Shader>(*String::Utf8Value(pass->ToObject()->Get(String::NewFromUtf8(isolate, "shader"))));
				
				new_pass.blend_state = AllocatedMemory::Instance().Construct<D3D11BlendState>();
				new_pass.blend_state->CreateFromJson(pass->ToObject()->Get(String::NewFromUtf8(isolate, "blend"))->ToObject());

				new_pass.depth_state = AllocatedMemory::Instance().Construct<D3D11DepthState>();
				new_pass.depth_state->CreateFromJson(pass->ToObject()->Get(String::NewFromUtf8(isolate, "depth"))->ToObject());

				new_passes.push_back(new_pass);
			}

			Technique technique = { name, new_passes };

			techniques_.emplace(name, technique);
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Effect::Apply(const std::string& tech, const unsigned int& p)
	{
		std::map<std::string, Technique>::iterator it = techniques_.find(tech);

		if (it != techniques_.end())
		{
			Technique& technique = it->second;
			std::vector<Pass>& passes = technique.passes;

			Pass& pass = passes.at(p);
			if (pass.shader != nullptr && pass.shader->is_valid() == true)
			{
				if (pass.shader != nullptr)
				{
					pass.shader->Set();
				}
			}
			else
			{
				pass.shader = nullptr;
			}

			pass.blend_state->Set();
			pass.depth_state->Set();

			return;
		}
		
		SNUFF_LOG_WARNING("Technique with name '" + tech + "' does not exist in the effect");
	}

	//---------------------------------------------------------------------------------------------------------
	unsigned int D3D11Effect::NumPasses(const std::string& tech)
	{
		std::map<std::string, Technique>::iterator it = techniques_.find(tech);

		if (it != techniques_.end())
		{
			return static_cast<unsigned int>(it->second.passes.size());
		}

		SNUFF_LOG_WARNING("Technique with name '" + tech + "' does not exist in the effect");
		return 0;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Effect::~D3D11Effect()
	{

	}
}