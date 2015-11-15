
#include "forward_renderer.h"
#include <d3d11.h>
#include <directxmath.h>
#include "shader_resource_manager.h"
#include "buffer_resource_manager.h"
#include "RenderTexture.h"
#include "OrthoMesh.h"
#include "d3d.h"
#include "BaseShader.h"
#include "normal_mapping_shader.h"
#include "Model.h"

namespace sz {

ForwardRenderer::ForwardRenderer(const unsigned int scr_height, 
  const unsigned int scr_width,
  const float scr_depth, const float scr_near, ID3D11Device* device,
  HWND hwnd, ConstBufManager *buf_man, ShaderManager *sha_man) :
  Renderer(scr_height, scr_width, scr_depth, scr_near, device, hwnd, buf_man,
  sha_man)
{

}

void ForwardRenderer::Render(D3D *d3d, Camera *cam,
  std::vector<Light> *lights) {
  // Render scene to a target
  RenderToTexture(*render_target_main_, d3d, cam, lights);

  d3d->BeginScene(0.39f, 0.58f, 0.92f, 1.0f);
  // Render the main target on the back buffer
  XMMATRIX base_view_matrix;
  cam->GetBaseViewMatrix(base_view_matrix);
  RenderToBackBuffer(*render_target_main_, d3d, base_view_matrix);

  // Present the rendered final frame to the screen.
  d3d->EndScene();
}

void ForwardRenderer::RenderToTexture(RenderTexture &target, D3D *d3d,
  Camera *cam, std::vector<Light> *lights) {
  // Set the render target to be the main render target
  target.SetRenderTarget(d3d->GetDeviceContext());

  // Clear the render to texture.
  target.ClearRenderTarget(d3d->GetDeviceContext(),
    0.0f, 0.0f, 0.0f, 1.0f);

  XMMATRIX world_matrix, view_matrix, projection_matrix;

  // Set per-frame shader paramters
  BaseShader *shader = sha_man_->GetShader("normal_mapping_shader");
  if (shader != nullptr) {
    NormalMappingShader *normal_map_shader =
      static_cast<NormalMappingShader*>(shader);
    normal_map_shader->SetShaderFrameParameters(d3d->GetDeviceContext(),
      *lights, cam);
  }

  // Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
  d3d->GetWorldMatrix(world_matrix);
  cam->GetViewMatrix(view_matrix);
  projection_matrix = target.GetProjectionMatrix();


  XMMATRIX model_transform = XMMatrixScaling(0.1f, 0.1f, 0.1f) /**
    XMMatrixTranslation(10.f, -20.f, 0.f)*/;
  // Create a base shader
  shader = nullptr;

  // For all the models
  for (Model *model : models_) {
    model->SendData(d3d->GetDeviceContext());

    // For all the meshes in the model
    //std::stack<size_t> alpha_blended_meshes;
    // For all the entries in the map
    for (auto map_pair : model->meshes_by_material()) {
      MatMeshPair &pair = map_pair.second;

      // Get the shader associated
      shader = sha_man_->GetShader(
        pair.first->shader_name);
      if (shader == nullptr) {
        continue;
      }

      // Set the parameters for this shader
      shader->SetShaderParameters(d3d->GetDeviceContext(),
        model_transform, view_matrix, projection_matrix,
        *(pair.first));
      // Set DX shaders and input layout
      shader->SetInputLayoutAndShaders(d3d->GetDeviceContext());
      shader->SetSamplers(d3d->GetDeviceContext());

      // For all the meshes associated with it
      for (BaseMesh *mesh : pair.second) {
        shader->Render(d3d->GetDeviceContext(),
          mesh->GetIndicesSize(), mesh->index_offset(),
          mesh->vertex_offset());
      }
    }
  }
 
  // For all the meshes which do not belong to a model
  //std::stack<size_t> alpha_blended_meshes;
  // For all the entries in the map
  //for (auto map_pair : meshes_by_material_) {
  //  MatMeshPair &pair = map_pair.second;

  //  // Get the shader associated
  //  shader = sha_man_->GetShader(
  //    pair.first->shader_name);
  //  if (shader == nullptr) {
  //    continue;
  //  }

  //  // Set the parameters for this shader
  //  shader->SetShaderParameters(d3d->GetDeviceContext(),
  //    model_transform, view_matrix, projection_matrix,
  //    *(pair.first));
  //  // Set DX shaders and input layout
  //  shader->SetInputLayoutAndShaders(d3d->GetDeviceContext());
  //  shader->SetSamplers(d3d->GetDeviceContext());

  //  // For all the meshes associated with it
  //  for (BaseMesh *mesh : pair.second) {
  //    mesh->SendData(d3d->GetDeviceContext());
  //    shader->Render(d3d->GetDeviceContext(),
  //      mesh->GetIndexCount());
  //  }
  //}

}

} // namespace sz