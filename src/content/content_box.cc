#include "../content/content_box.h"
#include "../content/content_manager.h"
#include "../platform/platform_text_file.h"
#include "../application/game.h"

using namespace v8;

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  Box::Box() :
    Content(ContentTypes::kBox)
  {
    for (int i = 0; i < 9; ++i)
    {
      files_.emplace(static_cast<ContentTypes>(i), std::vector<std::string>());
    }
  }

  //-------------------------------------------------------------------------------------------
  void Box::Load(const std::string& path)
  {
    TextFile file;
    bool success = file.Open(Game::Instance()->path() + "/" + path);

    if (success == true)
    {
      std::string stringified = file.Read();
      JSStateWrapper* wrapper = JSStateWrapper::Instance();
      Isolate* isolate = wrapper->isolate();

      HandleScope scope(isolate);

      TryCatch try_catch;
      Handle<Value> json = JSON::Parse(String::NewFromUtf8(isolate, stringified.c_str()));

      if (json.IsEmpty() == true)
      {
        SNUFF_LOG_ERROR("Could not parse box file '" + path + "'");
        std::string error;
        bool has_error = wrapper->GetException(&try_catch, &error);

        if (has_error == true)
        {
          SNUFF_LOG_ERROR(error);
        }

        return;
      }

      if (json->IsArray() == false)
      {
        SNUFF_LOG_ERROR("The box '" + path + "' is not of an array type, aborting");
        return;
      }

      Handle<Object> arr = json->ToObject();
      Handle<Value> item;
      for (unsigned int i = 0; i < arr->GetPropertyNames()->Length(); ++i)
      {
        item = arr->Get(i);

        if (item.IsEmpty() == false && item->IsObject() == true)
        {
          LoadItem(item->ToObject(), isolate, i, path);
          continue;
        }

        SNUFF_LOG_ERROR("Item at index '" + std::to_string(i) + "' in box '" + path + "' was not of an object type, skipping");
      }
    }
    else
    {
      SNUFF_LOG_ERROR("Could not open box file '" + path + "'");
    }
  }

  //-------------------------------------------------------------------------------------------
  void Box::LoadItem(const Handle<Object>& obj, Isolate* isolate, const int& idx, const std::string& path)
  {
    Handle<Value> type = obj->Get(String::NewFromUtf8(isolate, "type"));
    Handle<Value> file = obj->Get(String::NewFromUtf8(isolate, "path"));

    if (type.IsEmpty() == true || type->IsUndefined() == true || type->IsString() == false)
    {
      SNUFF_LOG_ERROR("'type' of item at index '" + std::to_string(idx) + "' in box '" + path + "' is not of a string type or undefined, skipping");
      return;
    }

    std::string t = *String::Utf8Value(type->ToString());

    if (file.IsEmpty() == true || file->IsUndefined() == true || file->IsString() == false)
    {
      SNUFF_LOG_ERROR("'path' of item at index '" + std::to_string(idx) + "' in box '" + path + "' is not of a string type or undefined, skipping");
      return;
    }

    std::string f = *String::Utf8Value(file->ToString());
    ContentTypes content_type = ContentManager::StringToType(t);
    ContentManager::Instance()->Notify(ContentManager::Events::kLoad, content_type, f);

    files_.find(content_type)->second.push_back(f);
  }

  //-------------------------------------------------------------------------------------------
  Box::~Box()
  {
    ContentManager* content_manager = ContentManager::Instance();
    for (ContentMap::iterator it = files_.begin(); it != files_.end(); ++it)
    {
      std::vector<std::string>& vec = it->second;

      for (unsigned int i = 0; i < vec.size(); ++i)
      {
        content_manager->Notify(ContentManager::Events::kUnload, it->first, vec.at(i));
      }
    }
  }
}