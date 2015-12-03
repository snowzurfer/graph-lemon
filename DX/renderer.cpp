
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
#include "DepthShader.h"
#include "Model.h"
#include "Light.h"
#include <sstream>
#include <string>
#include "gaussian_blur.h"

namespace sz {

Renderer::Renderer(const unsigned int scr_height, const unsigned int scr_width,
  const float scr_depth, const float scr_near, ID3D11Device* device,
  HWND hwnd, ConstBufManager *buf_man, ShaderManager *sha_man,
  const size_t lights_num) :
  meshes_by_material_(),
  use_post_process_(false),
  post_processer_(nullptr),
  models_(),
  render_target_main_(nullptr),
  render_target_depth_(nullptr),
  render_targets_depth_(lights_num),
  depth_target_w_(1024),
  depth_target_h_(1024),
  ortho_mesh_screen_(nullptr),
  render_target_main_mat_(nullptr),
  sha_man_(sha_man),
  buf_man_(buf_man),
  light_buff_(nullptr),
  camera_buff_(nullptr)
{
  // Create render targets 
  render_target_main_ = new RenderTexture(device,
    scr_width, scr_height, scr_near, scr_depth,
    "target_main");

    for (size_t i = 0; i < render_targets_depth_.size(); ++i) {
      std::string name;
      std::stringstream ss;
      ss << "target_depth_" << i;
      name = ss.str();
      render_targets_depth_[i] = new RenderTexture(device,
        depth_target_w_, depth_target_h_, scr_near, scr_depth,
        name.c_str());
    }

  // Create the ortho mesh
  ortho_mesh_screen_ = new OrthoMesh(device,
    scr_width, scr_height, 0, 0);

  // Create a material used to render the main render target to the screen
  render_target_main_mat_ = new Material();
  
  if (sha_man != nullptr) {
    {
      TextureShader *shader =
        new TextureShader(device, hwnd, *buf_man);
      if (!sha_man->AddShader("texture_shader", shader)) {
        delete shader;
        shader = nullptr;
      }
    }
    {
      DepthShader *shader =
        new DepthShader(device, hwnd, *buf_man);
      if (!sha_man->AddShader("depth_shader", shader)) {
        delete shader;
        shader = nullptr;
      }
    }
  }

  SetupPerFrameBuffers(device, buf_man, lights_num);

  post_processer_ = new sz::GaussBlur(scr_height, scr_width,
    scr_depth, scr_near, device, 
    hwnd, *buf_man_, sha_man_);
}

Renderer::~Renderer() {
  if (post_processer_ != nullptr) {
    delete post_processer_;
    post_processer_ = nullptr;
  }

  if (ortho_mesh_screen_!= nullptr) {
    delete ortho_mesh_screen_;
    ortho_mesh_screen_ = nullptr;
  }

  if (render_target_main_!= nullptr) {
    delete render_target_main_;
    render_target_main_ = nullptr;
  }

  for (RenderTexture *target : render_targets_depth_) {
    if (target != nullptr) {
      delete target;
      target = nullptr;
    }
  }
  render_targets_depth_.clear();
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

void Renderer::AddMeshesAndMaterials(std::vector<BaseMesh *> &meshes,
    const std::vector<Material> &materials) {
  for (size_t i = 0; i < meshes.size(); ++i) {
    // Get the mesh's material's crc name
    size_t m_id = meshes[i]->mat_id();
    UInt32 m_crc = materials[m_id].name_crc;

    // Add the mesh in the correct slot
    MeshesMatMap::iterator it = meshes_by_material_.find(m_crc);
    if (it != meshes_by_material_.end()) {
      MatMeshPair &pair =
        it->second;

      pair.second.push_back(meshes[i]);
    }
    else {
      MatMeshPair pair(&materials[m_id], std::vector<BaseMesh *>());

      pair.second.push_back(meshes[i]);

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
  BaseShader *shader = sha_man_->GetShader("texture_shader");
  TextureShader *texture_shader = static_cast<TextureShader *>(shader);
  if (texture_shader != nullptr) {
    shader->SetInputLayoutAndShaders(d3d->GetDeviceContext());
    texture_shader->SetShaderParameters(d3d->GetDeviceContext(),
      world_matrix, base_view_matrix, ortho_matrix,
      *render_target_main_mat_);
    texture_shader->Render(d3d->GetDeviceContext(),
      ortho_mesh_screen_->GetIndexCount(), 0);
  }

  d3d->TurnZBufferOn();

  texture_shader->CleanupTextures(d3d->GetDeviceContext());
}

void Renderer::SetupPerFrameBuffers(ID3D11Device *dev, ConstBufManager *buf_man,
    size_t lights_num) {
  D3D11_BUFFER_DESC lightBufferDesc;
  D3D11_BUFFER_DESC camBufferDesc;

  // Setup light buffer
  // Setup the description of the light dynamic constant buffer that is in the pixel shader.
  // Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
  lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  lightBufferDesc.ByteWidth = sizeof(sz::LightBufferType) * lights_num;
  lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  lightBufferDesc.MiscFlags = 0;
  lightBufferDesc.StructureByteStride = 0;

  // Create the constant buffer pointer so we can access the vertex shader constant 
  // buffer from within this class.
  light_buff_ = buf_man->CreateD3D11ConstBuffer("scene_lights_buffer",
    lightBufferDesc, dev);
  assert(light_buff_ != nullptr);

  // Setup cam buffer
  camBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  camBufferDesc.ByteWidth = sizeof(sz::CamBufferType);
  camBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  camBufferDesc.MiscFlags = 0;
  camBufferDesc.StructureByteStride = 0;

  // Create the constant buffer pointer so we can access the vertex shader constant 
  // buffer from within this class.
  camera_buff_ = buf_man->CreateD3D11ConstBuffer("scene_cam_buffer",
    camBufferDesc, dev);
  assert(camera_buff_ != nullptr);

}

void Renderer::SetFrameParameters(ID3D11DeviceContext* deviceContext,
    std::vector<Light> &lights, Camera *cam) {
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mapped_resource;
  sz::LightBufferType* light_ptr;
  sz::CamBufferType* camPtr;
  unsigned int bufferNumber;

  // Send light data to pixel and vertex shader
  result = deviceContext->Map(light_buff_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
  light_ptr = (sz::LightBufferType*)mapped_resource.pData;
  for (unsigned int i = 0; i < lights.size(); i++) {
    light_ptr[i].diffuse = lights[i].GetDiffuseColour();
    light_ptr[i].ambient = lights[i].GetAmbientColour();
    light_ptr[i].direction = XMFLOAT4(lights[i].GetDirection().x, 
      lights[i].GetDirection().y, lights[i].GetDirection().z, 0.f);
    light_ptr[i].specular = lights[i].GetSpecularColour();
    light_ptr[i].specular_power = lights[i].GetSpecularPower();
    light_ptr[i].attenuation = XMFLOAT4(lights[i].GetAttenuation().x, 
      lights[i].GetAttenuation().y, lights[i].GetAttenuation().z, 1.0);
    light_ptr[i].range = lights[i].GetRange();
    light_ptr[i].position = lights[i].GetPosition4();
    light_ptr[i].active = static_cast<unsigned int>(lights[i].active());
    light_ptr[i].spot_cutoff = lights[i].spot_cutoff();
    light_ptr[i].spot_exponent = lights[i].spot_exponent();
    light_ptr[i].view = XMMatrixTranspose(lights[i].GetViewMatrix());
    light_ptr[i].proj = XMMatrixTranspose(lights[i].GetProjectionMatrix());
  }
  deviceContext->Unmap(light_buff_, 0);
  bufferNumber = 0;
  deviceContext->PSSetConstantBuffers(bufferNumber, 1, &light_buff_);
  deviceContext->VSSetConstantBuffers(2, 1, &light_buff_);

  // Send camera data to vertex shader
  result = deviceContext->Map(camera_buff_, 0, D3D11_MAP_WRITE_DISCARD, 0,
    &mapped_resource);
  camPtr = (sz::CamBufferType*)mapped_resource.pData;
  camPtr->camPos = cam->GetPosition();
  deviceContext->Unmap(camera_buff_, 0);
  bufferNumber = 1;
  deviceContext->VSSetConstantBuffers(bufferNumber, 1, &camera_buff_);

}

} // namespace sz