
#include "buffer_resource_manager.h"

namespace szgrh {

ConstBufManager::ConstBufManager() :
    buffers_() {

}

ID3D11Buffer *ConstBufManager::CreateD3D11ConstBuffer(const std::string &name,
  const D3D11_BUFFER_DESC &desc,
  ID3D11Device* device) {
  // If the name passed is faulted
  if (name == "") {
    return nullptr;
  }

  // Look up the map to check if the buffer has already been created
  std::map<std::string, ID3D11Buffer *>::const_iterator it = buffers_.find(name);
  // If the buffer already exists 
  if (it != buffers_.end()) {
    // Return the pointer of the buffer
    return it->second;
  }

  // Create a buffer pointer
  ID3D11Buffer *buf_;
	HRESULT h_result = device->CreateBuffer(&desc, NULL, &buf_);

  // Check for errors
  if (h_result != S_OK) {
    return nullptr;
  }

  // Add buffer to list
  buffers_[name] = buf_;

  return buf_;
}

} // namespace szgrh