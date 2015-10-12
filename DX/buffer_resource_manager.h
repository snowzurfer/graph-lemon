//  A buffer manager class which manages the constant buffers needed by shaders
//	* Shaders are passed this class
//	* They ask this class for the buffer with a given name (string)
//	* The class looks up for the buffer with that name
//		* If already created, return the handle
//		* If not created, create it and return the handle

#ifndef _BUFFER_RESOURCE_MANAGER_H
#define _BUFFER_RESOURCE_MANAGER_H

// Third-party libraries
#include <d3d11.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <string>
#include <map>

namespace szgrh {
  
class ConstBufManager {
private:
  // The map of buffers relatively to their assigned name
  std::map<std::string, ID3D11Buffer *> buffers_;

public:
  // Ctor
  ConstBufManager();

  // Disable copy ctor and assignment operator
  ConstBufManager(const ConstBufManager &) = delete;
  ConstBufManager &operator=(const ConstBufManager &) = delete;
  ConstBufManager &operator=(const ConstBufManager &) const = delete;

  // Dtor
  ~ConstBufManager();

  // Create a buffer as explained at top of header file
  ID3D11Buffer *CreateD3D11ConstBuffer(const std::string &name,
    const D3D11_BUFFER_DESC &desc,
    ID3D11Device* device) const;
  
}; // class ConstBufManager

} // namespace szgrh

#endif