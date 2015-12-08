// Class which acts as the basis for developing
// different types of rendering

#ifndef _RENDERER_H
#define _RENDERER_H

#include <utility>
#include <tuple>
#include <vector>
#include <map>
#include "Material.h"
#include "BaseMesh.h"
#include "abertay_framework.h"
#include <directxmath.h>

class RenderTexture;
class OrthoMesh;
class D3D;
class Camera;
struct ID3D11Device;
class Light;
class Model;
class Timer;
namespace sz {
  class ConstBufManager;
  class ShaderManager;
  class PostProcess;
}

namespace sz{

class Renderer {
public:
  // Ctor
  Renderer(const unsigned int scr_height, const unsigned int scr_width,
    const float scr_depth, const float scr_near, ID3D11Device* device,
    HWND hwnd, ConstBufManager *buf_man, ShaderManager *sha_man,
    const size_t lights_num, const Timer &timer);

  // Dtor
  virtual ~Renderer();

  void AddMeshesAndMaterials(std::vector<BaseMesh> &meshes,
    const std::vector<Material> &materials);
  void AddMeshAndMaterial(BaseMesh &mesh, const Material &material);
  void AddModel(Model *model);
  void AddMeshesAndMaterials(std::vector<BaseMesh *> &meshes,
    const std::vector<Material> &materials);
  virtual void Render(D3D *d3d, Camera *cam, std::vector<Light> *lights) = 0;

  // Called when it's necessary to switch to using tessellation
  virtual void UpdateTessellation(ID3D11DeviceContext* deviceContext) {};
  virtual void UpdateVertexManipulation(ID3D11DeviceContext *deviceContext) {};
  inline bool IsChangeTessellationNecessary() const {
    return prev_tessellate_check_value_ != tessellate_check_;
  }
  inline bool IsChangeWavesNecessary() const {
    return  prev_vertex_manip_check_value_ != vertex_manip_check_;
  }

  // Parameters which can be set by the user via ImGui
  float tessellation_value;
  float tessellation_distance;
  float waves_amplitude;
  float waves_frequency;
protected:
  // Used to batch render by material
  typedef std::map<UInt32, std::pair<const Material *, std::vector<BaseMesh *>>>
    MeshesMatMap;
  typedef std::pair<const Material *, std::vector<BaseMesh *>>
    MatMeshPair;
  MeshesMatMap meshes_by_material_;

  // Whether to apply post-processing to the final image
  bool use_post_process_;
  sz::PostProcess *post_processer_;

  // Whether to show tessellation
  bool tessellate_check_;
  bool prev_tessellate_check_value_;
  bool tessellate_;

  // Whether to show vertex manipulation 
  bool vertex_manip_check_;
  bool prev_vertex_manip_check_value_;
  bool manip_vertices_;

  // Models batch meshes by material internally
  std::vector<Model *> models_;

  // Shared by all the rendering modes
  RenderTexture *render_target_main_;
  RenderTexture *render_target_depth_;
  std::vector<RenderTexture *> render_targets_depth_;
  const size_t depth_target_w_, depth_target_h_;
  OrthoMesh *ortho_mesh_screen_;
  Material *render_target_main_mat_;

  // References to the shaders and buffers managers
  ShaderManager *sha_man_;
  ConstBufManager *buf_man_;

  // Per-frame buffers
  ID3D11Buffer* light_buff_;
  ID3D11Buffer* camera_buff_;
  ID3D11Buffer* tessellation_buf_;
  ID3D11Buffer* time_buf_;

  // Reference to the app timer
  const Timer &timer_;

  // Render to the back buffer from a source render target
  void RenderToBackBuffer(const RenderTexture &source, D3D *d3d,
    const XMMATRIX &base_view_matrix);

  // Setup buffers used per-frame
  void SetupPerFrameBuffers(ID3D11Device *dev, ConstBufManager *buf_man,
    size_t lights_num);

  // Set per-frame parameters of shaders
  void SetFrameParameters(ID3D11DeviceContext* deviceContext,
    std::vector<Light> &lights, Camera *cam);

}; // class Renderer

} // namespace sz

#endif