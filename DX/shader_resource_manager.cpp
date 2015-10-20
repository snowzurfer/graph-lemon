
#include "shader_resource_manager.h"
#include <algorithm>


namespace szgrh {

ShaderManager::ShaderManager() :
    shaders_() {
}

ShaderManager::~ShaderManager() {
  // Run through every element and free them
  std::for_each(shaders_.begin(), shaders_.end(),
    [&](std::pair<const std::string, BaseShader *> &n) {
      delete n.second;
      n.second = nullptr;
  });
}

bool ShaderManager::AddShader(const std::string &name, BaseShader *s) {
  // If the name passed is faulted
  if (name == "") {
    return false;
  }

  // If the ptr passed is not valid
  if (s == nullptr) {
    return false;
  }

  // Look up the map to check if the shader has already been created
  std::map<std::string, BaseShader *>::const_iterator it = shaders_.find(name);
  // If the buffer already exists 
  if (it != shaders_.end()) {
    // Inform that the operation didn' work out
    return false;
  }

  // Add buffer to list
  shaders_[name] = s;

  return true;
}

  
BaseShader *ShaderManager::GetShader(const std::string &name) {
  // If the name passed is faulted
  if (name == "") {
    return nullptr;
  }
  
  // Look up the map to check if the shader has already been created
  std::map<std::string, BaseShader *>::const_iterator it = shaders_.find(name);
  // If the buffer already exists 
  if (it != shaders_.end()) {
    return it->second;
  }

  return nullptr;

}


} // namespace szgrh
