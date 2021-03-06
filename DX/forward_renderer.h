#ifndef _FORWARD_RENDERER_H
#define _FORWARD_RENDERER_H

class RenderTexture;
class OrthoMesh;
class D3D;
struct ID3D11Device;
namespace sz {
  class ConstBufManager;
  class ShaderManager;
}

#include "renderer.h"

namespace sz {

class ForwardRenderer : public Renderer {
public:
  // Ctor
  ForwardRenderer(const unsigned int scr_height, const unsigned int scr_width,
    const float scr_depth, const float scr_near, ID3D11Device* device,
    HWND hwnd, ConstBufManager *buf_man, ShaderManager *sha_man,
    const size_t lights_num, const class Timer &timer);

  // Dtor
  ~ForwardRenderer() {};

  void Render(D3D *d3d, Camera *cam, std::vector<Light> *lights);

  void UpdateTessellation(ID3D11DeviceContext* deviceContext);
  void UpdateVertexManipulation(ID3D11DeviceContext *deviceContext);

private:

  // Render the scene to a texture target
  void RenderToTexture(RenderTexture &target, D3D *d3d,
    Camera *cam, std::vector<Light> *lights);

  // Render the scene's depth to a target from a given
  // light's perspective
  void RenderSceneDepthFromLight(RenderTexture &target, D3D *d3d, Light *light);

}; // class ForwardRenderer

} // namespace sz

#endif