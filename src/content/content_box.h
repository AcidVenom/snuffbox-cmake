#pragma once

#include "../content/content.h"
#include "../js/js_state_wrapper.h"

#include <map>
#include <vector>

namespace snuffbox
{
  /**
  * @class snuffbox::Box
  * @brief Used to load multiple files as a package
  * @author Daniël Konings
  */
  class Box : public Content
  {
  public:
    /// Default constructor
    Box();

    /**
    * @brief Loads a box file from a given path
    * @param[in] path (const std::string&) The path to the box file
    */
    void Load(const std::string& path);

    /**
    * @brief Loads a given item
    * @param[in] item (const v8::Handle<v8::Object>&) The item to load
    * @param[in] isolate (v8::Isolate*) The isolate currently in use
    * @param[in] idx (const int&) The index being loaded from
    * @param[in] path (const std::string&) The path being loaded from
    */
    void LoadItem(const v8::Handle<v8::Object>& item, v8::Isolate* isolate, const int& idx, const std::string& path);

    /// Default destructor
    virtual ~Box();

  private:
    typedef std::map<ContentTypes, std::vector<std::string>> ContentMap;
    ContentMap files_; //!< The files associated with this box file
  };
}