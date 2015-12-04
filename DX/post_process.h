// Class which acts as a basis for all the other post process
// effects.
// Inherit from it to create a new post process effect.
//
// Its main function accepts a render target by reference,
// and returns it after applying the post processing.

#ifndef _POST_PROCESS_H
#define _POST_PROCESS_H

#include <d3d11.h>
#include <directxmath.h>
#include <string>

// Forward declarations
class RenderTexture;
class D3D;

namespace sz {

class PostProcess {
public:
  void* operator new(size_t i)
  {
    return _mm_malloc(i, 16);
  }

  void operator delete(void* p)
  {
    _mm_free(p);
  }
  // Ctor
  PostProcess(const unsigned int scr_height, const unsigned int scr_width,
    const float scr_depth, const float scr_near) :
      scr_height_(scr_height),
      scr_width_(scr_width),
      scr_depth_(scr_depth),
      scr_near_(scr_near) {};

  // Dtor
  virtual ~PostProcess() {};

  // Post process function. Functionality explained in header
  virtual void ApplyPostProcess(RenderTexture &target, D3D *direct3D,
    const DirectX::XMMATRIX &base_view_matrix,
    const DirectX::XMMATRIX &world_matrix) = 0;

protected:
  unsigned int scr_height_;
  unsigned int scr_width_;
  float scr_depth_;
  float scr_near_;

}; // class PostProcess

} // namespace sz

#endif