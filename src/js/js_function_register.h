#pragma once

#include <string>
#include <v8.h>

namespace snuffbox
{
  /**
  * @struct snuffbox::JSFunctionRegister
  * @brief Used to construct an array of functions to register to the JavaScript state
  * @author Daniël Konings
  */
  struct JSFunctionRegister
  {
    std::string name; //!< The name of the callback
    v8::FunctionCallback callback; //!< The static function callback

    /**
    * @brief Registers the given functions to an object template
    * @param[in] funcs (JSFunctionRegister*) The listed functions
    * @param[in] num (const int&) The number of entries
    * @param[in] obj (const v8::Handle<v8::ObjectTemplate>&) The object template to register the functions to
    */
    static void Register(JSFunctionRegister* funcs, const int& num, const v8::Handle<v8::ObjectTemplate>& obj);

    /**
    * @brief Registers the given functions to an object
    * @param[in] funcs (JSFunctionRegister*) The listed functions
    * @param[in] num (const int&) The number of entries
    * @param[in] obj (const v8::Handle<v8::Object>&) The object to register the functions to
    */
    static void Register(JSFunctionRegister* funcs, const int& num, const v8::Handle<v8::Object>& obj);
  };
}