
#include "renderer.h"
#include <d3d11.h>
#include "shader_resource_manager.h"
#include "buffer_resource_manager.h"
#include "RenderTexture.h"
#include "OrthoMesh.h"
#include "d3d.h"
#include "Camera.h"
#include "abertay_framework.h"
#include "crc.h"
#include "TextureShader.h"
#include "Model.h"

namespace sz {

Renderer::Renderer(const unsigned int scr_height, const unsigned int scr_width,
  const float scr_depth, const float scr_near, ID3D11Device* device,
  HWND hwnd, ConstBufManager *buf_man, ShaderManager *sha_man) :
  meshes_by_material_(),
  models_(),
  render_target_main_(nullptr),
  ortho_mesh_screen_(nullptr),
  render_target_main_mat_(nullptr),
  sha_man_(sha_man),
  buf_man_(buf_man)
{
  // Create render targets 
  render_target_main_ = new RenderTexture(device,
    scr_width, scr_height, scr_near, scr_depth,
    "target_main");
  // Create the ortho mesh
  ortho_mesh_screen_ = new OrthoMesh(device,
    scr_width, scr_height, 0, 0);
  // Create a material used to render the main render target to the screen
  render_target_main_mat_ = new Material();
  
  if (sha_man != nullptr) {
    TextureShader *shader =
      new TextureShader(device, hwnd, *buf_man);
    if (!sha_man->AddShader("texture_shader", shader)) {
      delete shader;
      shader = nullptr;
    }
  }
}

Renderer::~Renderer() {
  if (ortho_mesh_screen_!= nullptr) {
    delete ortho_mesh_screen_;
    ortho_mesh_screen_ = nullptr;
  }

  if (render_target_main_!= nullptr) {
    delete render_target_main_;
    render_target_main_ = nullptr;
  }
}

void Renderer::AddMeshesAndMaterials(std::vector<BaseMesh> &meshes,
    const std::vector<Material> &materials) {
  for (size_t i = 0; i < meshes.size(); ++i) {
    // Get the mesh's material's crc name
    size_t m_id = meshes[i].mat_id();
    UInt32 m_crc = materials[m_id].name_crc;

    // Add the mesh in the correct slot
    MeshesMatMap::iterator it = meshes_by_material_.find(m_crc);
    if (it != meshes_by_material_.end()) {
      MatMeshPair &pair =
        it->second;

      pair.second.push_back(&(meshes[i]));
    }
    else {
      MatMeshPair pair(&materials[m_id], std::vector<BaseMesh *>());
      
      pair.second.push_back(&meshes[i]);

      meshes_by_material_[m_crc] = pair;
    }
  }
}

void Renderer::AddModel(Model *model) {
  models_.push_back(model);
}

void Renderer::AddMeshAndMaterial(BaseMesh &mesh,
    const Material &material) {
  // Get the mesh's material's crc name
  size_t m_id = mesh.mat_id();
  UInt32 m_crc = material.name_crc;

  // Add the mesh in the correct slot
  MeshesMatMap::iterator it = meshes_by_material_.find(m_crc);
  if (it != meshes_by_material_.end()) {
    MatMeshPair &pair =
      it->second;

    pair.second.push_back(&mesh);
  }
  else {
    MatMeshPair pair(&material, std::vector<BaseMesh *>());

    pair.second.push_back(&mesh);

    meshes_by_material_[m_crc] = pair;
  }
}

void Renderer::RenderToBackBuffer(const RenderTexture &source, D3D *d3d,
  const XMMATRIX &base_view_matrix) {
  XMMATRIX ortho_matrix, world_matrix;

  d3d->SetBackBufferRenderTarget();

  // Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
  d3d->GetWorldMatrix(world_matrix);
  d3d->GetOrthoMatrix(ortho_matrix);// ortho matrix for 2D rendering

  d3d->TurnZBufferOff();

  render_target_main_mat_->diffuse_texname = source.name();
  render_target_main_mat_->diffuse_texname_crc =
    abfw::CRC::GetICRC(render_target_main_mat_->diffuse_texname.c_str());
  ortho_mesh_screen_->SendData(d3d->GetDeviceContext());
  BaseShader *texture_shader = sha_man_->GetShader("texture_shader");
  if (texture_shader != nullptr) {
    texture_shader->SetShaderParameters(d3d->GetDeviceContext(),
      world_matrix, base_view_matrix, ortho_matrix,
      *render_target_main_mat_);
    texture_shader->Render(d3d->GetDeviceContext(),
      ortho_mesh_screen_->GetIndexCount(), 0);
  }

  d3d->TurnZBufferOn();

}

} // namespace sz