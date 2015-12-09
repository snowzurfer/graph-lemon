// MainApplication.cpp
#include "MainApplication.h"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <fstream>
#include <stack>
#include "Texture.h"
#include "gaussian_blur.h"
#include "renderer.h"
#include "forward_renderer.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>

MainApplication::MainApplication(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight,
  Input *in) :
  BaseApplication(hinstance, hwnd, screenWidth, screenHeight, in),
  m_Shader(nullptr),
  texture_shader_(nullptr),
  model_(nullptr),
  m_Mesh(nullptr),
  cube_mesh_(nullptr),
  lights_pt_meshes_(),
  geometrybox_shader_(nullptr),
  lights_pt_meshes_materials_(),
  lights_(kNumLights),
  buf_manager_(nullptr),
  sha_manager_(nullptr),
  waves_shader_(nullptr),
  normal_map_shader_(nullptr),
  renderer_(nullptr),
  screen_width_(screenWidth),
  screen_height_(screenHeight),
  apply_post_processing_(false),
  prev_time_(0.f),
  show_debug_imgui_(true),
  use_wireframe_mode_(false),
  prev_use_wireframe_mode_(false) {
  // Create Mesh object
  m_Mesh = new SphereMesh(m_Direct3D->GetDevice(), L"../res/DefaultDiffuse.png");
  Texture::Inst()->LoadTexture(m_Direct3D->GetDevice(),
    m_Direct3D->GetDeviceContext(), "../res/DefaultDiffuse.png");
  Texture::Inst()->LoadTexture(m_Direct3D->GetDevice(),
    m_Direct3D->GetDeviceContext(), "../res/DefaultNormal.png");

  // Create a cube mesh
  cube_mesh_ = new CubeMesh(m_Direct3D->GetDevice(), L"../res/bunny.png", 2);
  
  Texture::Inst()->LoadTexture(m_Direct3D->GetDevice(),
    m_Direct3D->GetDeviceContext(), "../res/bunny.png");
  Texture::Inst()->LoadTexture(m_Direct3D->GetDevice(),
    m_Direct3D->GetDeviceContext(), "../res/bunny_bump.png");

  // Create the buffers manager
  buf_manager_ = new sz::ConstBufManager();
  // Create the shaders manager
  sha_manager_ = new sz::ShaderManager();

  // Create necessary shaders for meshes which are not loaded by a model
  m_Shader = new LightShader(m_Direct3D->GetDevice(), hwnd, *buf_manager_,
    kNumLights);
  //waves_shader_ = new WavesVertexDeformShader(m_Direct3D->GetDevice(), hwnd,
  //  *buf_manager_, kNumLights);
  normal_map_shader_ = new NormalMappingShader(m_Direct3D->GetDevice(), hwnd,
    *buf_manager_, kNumLights);
  texture_shader_ = new TextureShader(m_Direct3D->GetDevice(), hwnd, *buf_manager_);
  geometrybox_shader_ = new GeometryBoxShader(m_Direct3D->GetDevice(), hwnd,
    *buf_manager_);
  // No need to check for duplicates, geometrybox shaders are only created here
  sha_manager_->AddShader("geometrybox_shader", geometrybox_shader_);
  sz::Material lights_pt_meshes_material;
  lights_pt_meshes_material.shader_name = "geometrybox_shader";
  lights_pt_meshes_material.name = "light_meshes_material";
  lights_pt_meshes_material.name_crc = abfw::CRC::GetICRC("light_meshes_material");
  lights_pt_meshes_materials_.push_back(lights_pt_meshes_material);

  // Setup lights
  for (size_t i = 0; i < kNumLights; i++) {
    //lights_.push_back(Light());
    //lights_[i].SetPosition(0.f, 7.f, 0.f, 0.f);
    // Set the light values
    lights_[i].SetDiffuseColour(1.f, 1.f, 1.f, 1.f);
    lights_[i].SetSpecularColour(1.f, 1.f, 1.f, 1.f);
    lights_[i].SetSpecularPower(4.f);
    lights_[i].SetAttenuation(0.95f, 0.f, 0.f);
    lights_[i].SetRange(300.f);
    lights_[i].set_active(false);
    lights_[i].SetDirection(1.f, -0.3f, -0.1f);
    // Set ambient for one light only 
    if (i == 0) {
      lights_[i].SetAmbientColour(0.1f, 0.1f, 0.1f, 1.f);
      lights_[i].SetPosition(-60.f, 60.f, 0.f, 0.f);
      lights_[i].SetDiffuseColour(1.f, 1.f, 1.f, 1.f);
      lights_[i].SetDirection(1.f, 0.3f, -0.6f);
      lights_[i].set_spot_cutoff(static_cast<float>(M_PI_4 * 1.3));
      lights_[i].set_spot_exponent(2.f);
      lights_[i].set_active(true);
    }
    if (i == 1) {
      lights_[i].SetAmbientColour(0.1f, 0.1f, 0.1f, 1.f);
      lights_[i].SetPosition(-60.f, 60.f, 10.f, 0.f);
      lights_[i].SetDiffuseColour(1.f, 1.f, 1.f, 1.f);
      lights_[i].SetDirection(1.f, 0.f, 0.6f);
      lights_[i].set_spot_cutoff(static_cast<float>(M_PI_4 * 1.3));
      lights_[i].set_spot_exponent(2.f);
      lights_[i].set_active(true);
    }
    if (i == 2) {
      lights_[i].SetAmbientColour(0.1f, 0.1f, 0.1f, 1.f);
      lights_[i].SetPosition(40.f, 50.f, 0.f, 0.f);
      lights_[i].SetDiffuseColour(1.f, 1.f, 1.f, 1.f);
      lights_[i].set_active(true);
      lights_[i].SetAttenuation(0.5f, 0.03f, 0.f);
      lights_[i].SetDirection(-1.f, -0.5f, 0.0f);
      lights_[i].set_spot_cutoff(static_cast<float>(M_PI_4 * 1.2));
      lights_[i].set_spot_exponent(2.f);
    }
    if (i == 3) {
      lights_[i].SetAmbientColour(0.5f, 0.5f, 0.5f, 1.f);
      lights_[i].SetPosition(0.f, 0.f, 0.f, 0.f);
      lights_[i].SetDiffuseColour(0.5f, 0.5f, 0.5f, 1.f);
      lights_[i].set_active(true);
      lights_[i].SetAttenuation(0.4f, 0.155f, 0.f);
    }

    // Create the lights point meshes
    lights_pt_meshes_.push_back(new PointMesh(m_Direct3D->GetDevice()));
    lights_pt_meshes_[i]->set_transform(DirectX::XMMatrixTranslation(
      lights_[i].GetPosition3().x,
      lights_[i].GetPosition3().y,
      lights_[i].GetPosition3().z
      ));
    lights_pt_meshes_[i]->set_mat_id(0);
  }

  model_ = new Model();
  // Attempt loading a model 
  {
    std::ifstream ifs("../res/sponza/sponza_proc.szg",
      std::ios::in | std::ios::binary);
    boost::archive::binary_iarchive bia(ifs);
    bia >> (*model_);
  }
  // Initialise the model
  model_->Init(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
    hwnd, *buf_manager_, kNumLights, *sha_manager_);

  renderer_ = new sz::ForwardRenderer(screenHeight, screenWidth,
    SCREEN_DEPTH, SCREEN_NEAR, m_Direct3D->GetDevice(),
    hwnd, buf_manager_, sha_manager_, kNumLights, *m_Timer);
  //renderer_->AddMeshesAndMaterials(model_->meshes_, model_->materials_);
  renderer_->AddModel(model_);
  renderer_->AddMeshesAndMaterials(lights_pt_meshes_,
    lights_pt_meshes_materials_);


}


MainApplication::~MainApplication() {

  if (renderer_ != nullptr) {
    delete renderer_;
    renderer_ = nullptr;
  }

  // Run base application deconstructor
  BaseApplication::~BaseApplication();

  // Release the Direct3D object.
  if (m_Mesh) {
    delete m_Mesh;
    m_Mesh = 0;
  }

  if (m_Shader) {
    delete m_Shader;
    m_Shader = 0;
  }

  if (model_ != nullptr) {
    delete model_;
    model_ = nullptr;
  }

  if (cube_mesh_ != nullptr) {
    delete cube_mesh_;
    cube_mesh_ = nullptr;
  }

  if (sha_manager_ != nullptr) {
    delete sha_manager_;
    sha_manager_ = nullptr;
  }

  if (buf_manager_ != nullptr) {
    delete buf_manager_;
    buf_manager_ = nullptr;
  }

  if (waves_shader_ != nullptr) {
    delete waves_shader_;
    waves_shader_ = nullptr;
  }

  if (normal_map_shader_ != nullptr) {
    delete normal_map_shader_;
    normal_map_shader_ = nullptr;
  }

  //if (m_Light) {
  //  delete m_Light;
  //  m_Light = nullptr;
  //}
}


bool MainApplication::Frame() {
  bool result = BaseApplication::Frame();
  if (!result) {
    return false;
  }
  
  ImGui::SetNextWindowSize(ImVec2(500,230));
  ImGui::Begin("Debug Tools", &show_debug_imgui_);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::Checkbox("Wireframe mode", &use_wireframe_mode_);
  if (use_wireframe_mode_ != prev_use_wireframe_mode_) {
    m_Direct3D->ToggleWireFrame();

    prev_use_wireframe_mode_ = use_wireframe_mode_;
  }
  ImGui::SliderFloat("Tessellation factor", &renderer_->tessellation_value,
    1.f, 10.f);
  ImGui::SliderFloat("Tessellation distance", &renderer_->tessellation_distance,
    1.f, 300.f);

  // Update camera
  m_Camera->Update();

  // Update lights
  for (size_t i = 0; i < lights_.size(); i++) {
    lights_[i].GenerateProjectionMatrix(SCREEN_NEAR, SCREEN_DEPTH);
    lights_[i].GenerateViewMatrixFromDirection();
    lights_pt_meshes_[i]->set_transform(XMMatrixTranslation(
      lights_[i].GetPosition3().x,
      lights_[i].GetPosition3().y,
      lights_[i].GetPosition3().z
      ));
  }


  ImGui::SliderFloat("Wave amplitude", &renderer_->waves_amplitude,
    1.f, 100.f);
  ImGui::SliderFloat("Wave frequency", &renderer_->waves_frequency,
    0.f, 1.f);

  // Render the graphics.
  result = Render();
  if (!result) {
    return false;
  }
  

  if(renderer_->IsChangeTessellationNecessary()) {
    renderer_->UpdateTessellation(m_Direct3D->GetDeviceContext());
  }
  if(renderer_->IsChangeWavesNecessary()) {
    renderer_->UpdateVertexManipulation(m_Direct3D->GetDeviceContext());
  }
  // Increment the time counter
  prev_time_ += m_Timer->GetTime();

  return true;
}

bool MainApplication::Render() {
  renderer_->Render(m_Direct3D, m_Camera, &lights_);
  m_Direct3D->SetBackBufferRenderTarget();

  //bool show_test_window = true;
  //  bool show_another_window = false;
  //  ImVec4 clear_col = ImColor(114, 144, 154);
  //
  //          static float f = 0.0f;
  //          ImGui::Text("Hello, world!");
  //          ImGui::ColorEdit3("clear color", (float*)&clear_col);
  //          if (ImGui::Button("Test Window")) show_test_window ^= 1;
  //          if (ImGui::Button("Another Window")) show_another_window ^= 1;
  //          ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  m_Direct3D->TurnZBufferOff();


  ImGui::End();
  ImGui::Render();

  m_Direct3D->TurnZBufferOn();
  m_Direct3D->TurnOffAlphaBlending();
  m_Direct3D->SetDefaultRasterizerState();

  // Present the rendered final frame to the screen.
  m_Direct3D->EndScene();

  return true;
}