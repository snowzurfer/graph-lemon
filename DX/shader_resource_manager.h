//  A shader manager class which manages the shaders needed by materials 
//  * Shaders are passed this class
//  * They ask this class for the buffer with a given name (string)
//  * The class looks up for the buffer with that name
//    * If already created, return the handle
//    * If not created, create it and return the handle

#ifndef _SHADER_RESOURCE_MANAGER_H
#define _SHADER_RESOURCE_MANAGER_H

// Third-party libraries
#include <d3d11.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <string>
#include <unordered_map>
#include "BaseShader.h"

namespace sz {
  
class ShaderManager {
private:
  // The map of shaders relatively to their assigned name
  std::unordered_map<std::string, BaseShader*> shaders_;

public:
  // Ctor
  ShaderManager();

  // Disable copy ctor and assignment operator
  ShaderManager(const ShaderManager &) = delete;
  ShaderManager &operator=(const ShaderManager &) = delete;

  // Dtor
  ~ShaderManager();

  // Add a new shader
  bool AddShader(const std::string &name, BaseShader *s);

  // Get ref to a shader
  BaseShader *GetShader(const std::string &name);

  void CleanupShaderResources(ID3D11DeviceContext* deviceContext);
  void SetVertexManipulation(ID3D11DeviceContext* deviceContext, bool manipulate);


  
}; // class ShaderManager

} // namespace sz

#endif
