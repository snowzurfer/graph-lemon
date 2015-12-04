#ifndef _GAUSSIAN_BLUR_H
#define _GAUSSIAN_BLUR_H

#include <d3d11.h>
#include <directxmath.h>
#include "post_process.h"

// Forward declarations
class RenderTexture;
class OrthoMesh;
class GaussBlurHShader;
class GaussBlurVShader;
class TextureShader;
class D3D;
namespace sz {
  class ConstBufManager;
  class ShaderManager;
}

namespace sz {

class GaussBlur : public PostProcess {
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
  GaussBlur(const unsigned int scr_height, const unsigned int scr_width,
    const float scr_depth, const float scr_near, ID3D11Device* device,
    HWND hwnd, ConstBufManager &buf_man, ShaderManager *sha_man);

  // Dtor
  ~GaussBlur();

  // Post process function. Functionality explained in header
  void ApplyPostProcess(RenderTexture &target, D3D *direct3D,
    const DirectX::XMMATRIX &base_view_matrix,
    const DirectX::XMMATRIX &world_matrix);

private:
  // Render the target to a smaller render target, hence downsampling
  void DownSample(RenderTexture &target, D3D *direct3D,
    const DirectX::XMMATRIX &base_view_matrix,
    const DirectX::XMMATRIX &world_matrix);

  // Render the smaller target to a bigger render target, hence upsampling
  void UpSample(RenderTexture &target, D3D *direct3D,
    const DirectX::XMMATRIX &base_view_matrix,
    const DirectX::XMMATRIX &world_matrix);

  // Perform horizontal blurring for Gaussian blur
  void HorizontalBlur(RenderTexture &target, D3D *direct3D,
    const DirectX::XMMATRIX &base_view_matrix,
    const DirectX::XMMATRIX &world_matrix);

  // Perform horizontal blurring for Gaussian blur
  void VerticalBlur(RenderTexture &target, D3D *direct3D,
    const DirectX::XMMATRIX &base_view_matrix,
    const DirectX::XMMATRIX &world_matrix);

  // Render targets for blurring
  RenderTexture *render_target_downsample0_;
  RenderTexture *render_target_downsample1_;

  // Ortho meshes for post processing
  OrthoMesh *ortho_mesh_downsample_;
  OrthoMesh *ortho_mesh_upsample_;

  // Reference to the shaders manager
  ShaderManager *sha_man_;

}; // class GaussBlur

} // namespace sz

#endif
