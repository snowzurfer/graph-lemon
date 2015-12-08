
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
#include "buffer_types.h"
#include <cassert>
#include <imgui.h>
#include "gaussian_blur.h"
#include "Timer.h"

namespace sz {

ForwardRenderer::ForwardRenderer(const unsigned int scr_height, 
  const unsigned int scr_width,
  const float scr_depth, const float scr_near, ID3D11Device* device,
  HWND hwnd, ConstBufManager *buf_man, ShaderManager *sha_man,
  const size_t lights_num, const Timer &timer) :
  Renderer(scr_height, scr_width, scr_depth, scr_near, device, hwnd, buf_man,
  sha_man, lights_num, timer)
{
}

void ForwardRenderer::Render(D3D *d3d, Camera *cam,
  std::vector<Light> *lights) {
  sha_man_->CleanupShaderResources(d3d->GetDeviceContext());

  // Render scene from the lights' point of view
  for (size_t i = 0; i < lights->size(); ++i) {
    RenderSceneDepthFromLight(*(render_targets_depth_[i]), d3d, (&(*lights)[i]));
  }

  // Render scene to a target
  RenderToTexture(*render_target_main_, d3d, cam, lights);

  ImGui::Checkbox("Apply post processing", &use_post_process_);
  ImGui::Checkbox("Apply vertex manipulation", &vertex_manip_check_);
  ImGui::Checkbox("Apply tessellation", &tessellate_check_);

  if (use_post_process_) {
    sha_man_->CleanupShaderResources(d3d->GetDeviceContext());
    XMMATRIX base_view;
    cam->GetBaseViewMatrix(base_view);
    post_processer_->ApplyPostProcess(
      *render_target_main_,
      d3d,
      base_view,
      XMMatrixIdentity()
      );
  }

  d3d->BeginScene(0.39f, 0.58f, 0.92f, 1.0f);
  // Render the main target on the back buffer
  XMMATRIX base_view_matrix;
  cam->GetBaseViewMatrix(base_view_matrix);
  RenderToBackBuffer(*render_target_main_, d3d, base_view_matrix);

  // Present the rendered final frame to the screen.
  //d3d->EndScene();
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
  SetFrameParameters(d3d->GetDeviceContext(), *lights, cam);

  // Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
  d3d->GetWorldMatrix(world_matrix);
  cam->GetViewMatrix(view_matrix);
  projection_matrix = target.GetProjectionMatrix();

  XMMATRIX model_transform = XMMatrixScaling(0.1f, 0.1f, 0.1f) /**
    XMMatrixTranslation(10.f, -20.f, 0.f)*/;
  // Create a base shader
  BaseShader *shader = nullptr;
  BaseShader *prev_shader = nullptr;

  // For all the models
  for (Model *model : models_) {
    model->SendData(d3d->GetDeviceContext(), tessellate_);

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
      
      if (shader != prev_shader) {
        shader->SetInputLayoutAndShaders(d3d->GetDeviceContext());
        shader->SetSamplers(d3d->GetDeviceContext());
      }
      // Set the parameters for this shader
      shader->SetShaderParameters(d3d->GetDeviceContext(),
        model_transform, view_matrix, projection_matrix,
        *(pair.first));
      // Set DX shaders and input layout

      // For all the meshes associated with it
      for (BaseMesh *mesh : pair.second) {
        shader->Render(d3d->GetDeviceContext(),
          mesh->GetIndicesSize(), mesh->index_offset(),
          mesh->vertex_offset());
      }

      prev_shader = shader;
    }
  }

  // For all the meshes which do not belong to a model
  //std::stack<size_t> alpha_blended_meshes;
   //For all the entries in the map
  for (auto map_pair : meshes_by_material_) {
    MatMeshPair &pair = map_pair.second;

    // Get the shader associated
    shader = sha_man_->GetShader(
      pair.first->shader_name);
    if (shader == nullptr) {
      continue;
    }

    // Set the parameters for this shader
    // Set DX shaders and input layout
    shader->SetSamplers(d3d->GetDeviceContext());
    shader->SetInputLayoutAndShaders(d3d->GetDeviceContext());

    // For all the meshes associated with it
    for (BaseMesh *mesh : pair.second) {
      shader->SetShaderParameters(d3d->GetDeviceContext(),
        mesh->transform(), view_matrix, projection_matrix,
        *(pair.first));
      mesh->SendData(d3d->GetDeviceContext());
      shader->Render(d3d->GetDeviceContext(),
        mesh->GetIndexCount());

    }
  }

}

void ForwardRenderer::UpdateTessellation(ID3D11DeviceContext* deviceContext) {
  if (prev_tessellate_check_value_ != tessellate_check_) {
    tessellate_ = tessellate_check_;
    prev_tessellate_check_value_ = tessellate_;
    if (tessellate_check_ == true) {
      vertex_manip_check_ = false;
      prev_vertex_manip_check_value_ = false;
    }

    models_[0]->SetTessellation(deviceContext, sha_man_, tessellate_);
  }
}

void ForwardRenderer::UpdateVertexManipulation(ID3D11DeviceContext *deviceContext) {
  if (prev_vertex_manip_check_value_ != vertex_manip_check_) {
    manip_vertices_ = vertex_manip_check_;
    prev_vertex_manip_check_value_ = manip_vertices_;
    if (vertex_manip_check_ == true) {
      tessellate_check_ = false;
      prev_tessellate_check_value_ = false;
      tessellate_ = false;
    
      models_[0]->SetTessellation(deviceContext, sha_man_, tessellate_);
    }

    sha_man_->SetVertexManipulation(deviceContext, manip_vertices_);
  }

}
void ForwardRenderer::RenderSceneDepthFromLight(RenderTexture &target, D3D *d3d,
  Light *light) {
  target.SetRenderTarget(d3d->GetDeviceContext());

  // Clear the render to texture.
  target.ClearRenderTarget(d3d->GetDeviceContext(),
    0.0f, 0.0f, 0.0f, 1.0f);

  XMMATRIX world_matrix, view_matrix, projection_matrix;
  view_matrix = light->GetViewMatrix();
  projection_matrix = light->GetProjectionMatrix();
  d3d->GetWorldMatrix(world_matrix);

  XMMATRIX model_transform = XMMatrixScaling(0.1f, 0.1f, 0.1f) /**
    XMMatrixTranslation(10.f, -20.f, 0.f)*/;
  // Create a base shader
  BaseShader *shader = sha_man_->GetShader("depth_shader");
  if (shader == nullptr) {
    return;
  }

  // Set shaders parameters
  shader->SetShaderParameters(d3d->GetDeviceContext(),
    model_transform, view_matrix, projection_matrix,
    sz::Material()); // Does not matter which material is passed;
                    // It's not used.
  // Set DX shaders and input layout
  shader->SetInputLayoutAndShaders(d3d->GetDeviceContext());
  shader->SetSamplers(d3d->GetDeviceContext());

  // For all the models
  for (Model *model : models_) {
    model->SendData(d3d->GetDeviceContext());

    // For all the meshes in the model
    //std::stack<size_t> alpha_blended_meshes;
    // For all the entries in the map
    for (auto map_pair : model->meshes_by_material()) {
      MatMeshPair &pair = map_pair.second;

    //shader->Render(d3d->GetDeviceContext(),
    //  pair.second[0]->GetIndicesSize(), pair.second[0]->index_offset(),
    //  pair.second[0]->vertex_offset());
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
   //For all the entries in the map
  for (auto map_pair : meshes_by_material_) {
    MatMeshPair &pair = map_pair.second;

    // Skip rendering of depth of geometry positional boxes
    if (pair.first->shader_name == "geometrybox_shader") {
      continue;
    }

    // Set the parameters for this shader
    // Set DX shaders and input layout
    shader->SetSamplers(d3d->GetDeviceContext());
    shader->SetInputLayoutAndShaders(d3d->GetDeviceContext());

    // For all the meshes associated with it
    for (BaseMesh *mesh : pair.second) {
      shader->SetShaderParameters(d3d->GetDeviceContext(),
        mesh->transform(), view_matrix, projection_matrix,
        *(pair.first));
      mesh->SendData(d3d->GetDeviceContext());
      shader->Render(d3d->GetDeviceContext(),
        mesh->GetIndexCount());
    }
  }

}


} // namespace sz