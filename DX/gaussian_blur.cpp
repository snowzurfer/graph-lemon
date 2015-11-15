
#include "gaussian_blur.h"
#include "RenderTexture.h"
#include "OrthoMesh.h"
#include "shader_resource_manager.h"
#include "buffer_resource_manager.h"
#include "TextureShader.h"
#include "gauss_blur_h_shader.h"
#include "gauss_blur_v_shader.h"
#include "d3d.h"

namespace sz {

GaussBlur::GaussBlur(const unsigned int scr_height, const unsigned int scr_width,
  const float scr_depth, const float scr_near, ID3D11Device* device,
  HWND hwnd, ConstBufManager &buf_man, ShaderManager *sha_man) :
    PostProcess(scr_height, scr_width, scr_depth, scr_near),
    render_target_downsample0_(nullptr),
    render_target_downsample1_(nullptr),
    ortho_mesh_downsample_(nullptr),
    ortho_mesh_upsample_(nullptr),
    sha_man_(sha_man) 
{
  render_target_downsample0_ = new RenderTexture(device,
    scr_width / 2, scr_height / 2, scr_near, scr_depth,
    "target_downsample0");
  render_target_downsample1_ = new RenderTexture(device,
    scr_width / 2, scr_height / 2, scr_near, scr_depth,
    "target_downsample1");

  // Create the ortho mesh
  ortho_mesh_upsample_ = new OrthoMesh(device,
    scr_width, scr_height, 0, 0);
  ortho_mesh_downsample_ = new OrthoMesh(device,
    scr_width / 2, scr_height / 2, 0, 0);

  if (sha_man != nullptr) {
    {
      TextureShader *shader =
        new TextureShader(device, hwnd, buf_man);
      if (!sha_man->AddShader("texture_shader", shader)) {
        delete shader;
        shader = nullptr;
      }
    }
    {
      GaussBlurHShader *shader =
        new GaussBlurHShader(device, hwnd, buf_man);
      if (!sha_man->AddShader("gauss_blur_h_shader", shader)) {
        delete shader;
        shader = nullptr;
      }
    }
    {
      GaussBlurVShader *shader =
        new GaussBlurVShader(device, hwnd, buf_man);
      if (!sha_man->AddShader("gauss_blur_v_shader", shader)) {
        delete shader;
        shader = nullptr;
      }
    }
  }
  
}

// Dtor
GaussBlur::~GaussBlur() {
  if (ortho_mesh_downsample_ != nullptr) {
    delete ortho_mesh_downsample_;
    ortho_mesh_downsample_ = nullptr;
  }
  if (ortho_mesh_upsample_ != nullptr) {
    delete ortho_mesh_upsample_;
    ortho_mesh_upsample_ = nullptr;
  }

  if (render_target_downsample1_ != nullptr) {
    delete render_target_downsample1_;
    render_target_downsample1_ = nullptr;
  }
  if (render_target_downsample0_ != nullptr) {
    delete render_target_downsample0_;
    render_target_downsample0_ = nullptr;
  }

}

void GaussBlur::DownSample(RenderTexture &target, D3D *direct3D,
  const DirectX::XMMATRIX &base_view_matrix,
  const DirectX::XMMATRIX &world_matrix) {
  // Set the render target to be the downscaling render target
  render_target_downsample0_->SetRenderTarget(direct3D->GetDeviceContext());

  // Clear the render to texture.
  render_target_downsample0_->ClearRenderTarget(direct3D->GetDeviceContext(),
    0.0f, 0.0f, 0.0f, 1.0f);

  XMMATRIX ortho_matrix =
    render_target_downsample0_->GetOrthoMatrix();// ortho matrix for 2D rendering

  // Create a mock material
  sz::Material mock_material;

  mock_material.diffuse_texname = target.name();
  ortho_mesh_downsample_->SendData(direct3D->GetDeviceContext());
  BaseShader *texture_shader = sha_man_->GetShader("texture_shader");
  if (texture_shader != nullptr) {
    texture_shader->SetShaderParameters(direct3D->GetDeviceContext(),
      world_matrix, base_view_matrix, ortho_matrix,
      mock_material);
    texture_shader->Render(direct3D->GetDeviceContext(),
      ortho_mesh_downsample_->GetIndexCount(), 0);
  }

}

void GaussBlur::UpSample(RenderTexture &target, D3D *direct3D,
    const DirectX::XMMATRIX &base_view_matrix,
    const DirectX::XMMATRIX &world_matrix) {
  // Set the render target to be the main render target
  target.SetRenderTarget(direct3D->GetDeviceContext());

  // Clear the render to texture.
  target.ClearRenderTarget(direct3D->GetDeviceContext(),
    0.0f, 0.0f, 0.0f, 1.0f);

  XMMATRIX ortho_matrix =
    target.GetOrthoMatrix();// ortho matrix for 2D rendering

  // Create a mock material
  sz::Material mock_material;

  mock_material.diffuse_texname = render_target_downsample0_->name();
  ortho_mesh_upsample_->SendData(direct3D->GetDeviceContext());
  BaseShader *texture_shader = sha_man_->GetShader("texture_shader");
  if (texture_shader != nullptr) {
    texture_shader->SetShaderParameters(direct3D->GetDeviceContext(),
      world_matrix, base_view_matrix, ortho_matrix,
      mock_material);
    texture_shader->Render(direct3D->GetDeviceContext(),
      ortho_mesh_upsample_->GetIndexCount(), 0);
  }

}

void GaussBlur::HorizontalBlur(RenderTexture &target, D3D *direct3D,
    const DirectX::XMMATRIX &base_view_matrix,
    const DirectX::XMMATRIX &world_matrix) {
  // Set the render target
  render_target_downsample1_->SetRenderTarget(direct3D->GetDeviceContext());

  // Clear the render to texture.
  render_target_downsample1_->ClearRenderTarget(direct3D->GetDeviceContext(),
    0.0f, 0.0f, 0.0f, 1.0f);

  XMMATRIX ortho_matrix =
    render_target_downsample1_->GetOrthoMatrix();// ortho matrix for 2D rendering

  // Create a mock material
  sz::Material mock_material;

  mock_material.diffuse_texname = render_target_downsample0_->name();
  ortho_mesh_downsample_->SendData(direct3D->GetDeviceContext());
  BaseShader *shader = sha_man_->GetShader("gauss_blur_h_shader");
  if (shader != nullptr) {
    GaussBlurHShader *gauss_shader = static_cast<GaussBlurHShader *>(shader);
    gauss_shader->SetShaderParameters(direct3D->GetDeviceContext(),
      world_matrix, base_view_matrix, ortho_matrix,
      mock_material, scr_width_ * 0.5f);
    gauss_shader->Render(direct3D->GetDeviceContext(),
      ortho_mesh_downsample_->GetIndexCount(), 0);
  }

}

void GaussBlur::VerticalBlur(RenderTexture &target, D3D *direct3D,
    const DirectX::XMMATRIX &base_view_matrix,
    const DirectX::XMMATRIX &world_matrix) {
  // Set the render target
  render_target_downsample0_->SetRenderTarget(direct3D->GetDeviceContext());

  // Clear the render to texture.
  render_target_downsample0_->ClearRenderTarget(direct3D->GetDeviceContext(),
    0.0f, 0.0f, 0.0f, 1.0f);

  XMMATRIX ortho_matrix =
    render_target_downsample0_->GetOrthoMatrix();// ortho matrix for 2D rendering

  // Create a mock material
  sz::Material mock_material;

  mock_material.diffuse_texname = render_target_downsample1_->name();
  ortho_mesh_downsample_->SendData(direct3D->GetDeviceContext());
  BaseShader *shader = sha_man_->GetShader("gauss_blur_v_shader");
  if (shader != nullptr) {
    GaussBlurVShader *gauss_shader = static_cast<GaussBlurVShader *>(shader);
    gauss_shader->SetShaderParameters(direct3D->GetDeviceContext(),
      world_matrix, base_view_matrix, ortho_matrix,
      mock_material, scr_height_ * 0.5f);
    gauss_shader->Render(direct3D->GetDeviceContext(),
      ortho_mesh_downsample_->GetIndexCount(), 0);
  }

}

// Post process function. Functionality explained in header
void GaussBlur::ApplyPostProcess(RenderTexture &target, D3D *direct3D,
  const DirectX::XMMATRIX &base_view_matrix,
  const DirectX::XMMATRIX &world_matrix) {

  // Turn off the Z buffer to begin all 2D rendering.
  direct3D->TurnZBufferOff();

  // Downsample the texture
  DownSample(target, direct3D, base_view_matrix, world_matrix);

  HorizontalBlur(target, direct3D, base_view_matrix, world_matrix);
  VerticalBlur(target, direct3D, base_view_matrix, world_matrix);
  
  UpSample(target, direct3D, base_view_matrix, world_matrix);

  direct3D->TurnZBufferOn();

}


} // namespace sz