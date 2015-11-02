// Lab3.cpp
#include "Lab3.h"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <fstream>
#include <stack>
#include "Texture.h"

Lab3::Lab3(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, 
  Input *in) : 
    BaseApplication(hinstance, hwnd, screenWidth, screenHeight, in),
    texture_shader_(nullptr),
    model_(nullptr), cube_mesh_(nullptr),
    buf_manager_(nullptr), sha_manager_(nullptr),
    prev_time_(0.f),
    normal_map_shader_(nullptr), render_target_0_(nullptr),
    ortho_mesh_0_(nullptr) {
	// Create Mesh object
	m_Mesh = new SphereMesh(m_Direct3D->GetDevice(), L"../res/DefaultDiffuse.png");
	Texture::Inst()->LoadTexture(m_Direct3D->GetDevice(), 
    m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
	Texture::Inst()->LoadTexture(m_Direct3D->GetDevice(), 
    m_Direct3D->GetDeviceContext(), L"../res/DefaultNormal.png");

  // Create a cube mesh
  cube_mesh_ = new CubeMesh(m_Direct3D->GetDevice(), L"../res/bunny.png", 2);
	Texture::Inst()->LoadTexture(m_Direct3D->GetDevice(), 
    m_Direct3D->GetDeviceContext(), L"../res/bunny.png");
	Texture::Inst()->LoadTexture(m_Direct3D->GetDevice(), 
    m_Direct3D->GetDeviceContext(), L"../res/bunny_bump.png");

  // Create the buffers manager
  buf_manager_ = new szgrh::ConstBufManager();
  // Create the shaders manager
  sha_manager_ = new szgrh::ShaderManager();

	m_Shader = new LightShader(m_Direct3D->GetDevice(), hwnd, *buf_manager_, 
    kNumLights);

  //waves_shader_ = new WavesVertexDeformShader(m_Direct3D->GetDevice(), hwnd,
  //  *buf_manager_, kNumLights);

  normal_map_shader_ = new NormalMappingShader(m_Direct3D->GetDevice(), hwnd,
    *buf_manager_, kNumLights);

  texture_shader_ = new TextureShader(m_Direct3D->GetDevice(), hwnd, *buf_manager_) ;

  for (unsigned int i = 0; i < kNumLights; i++) {
    lights_.push_back(Light());
    //lights_[i].SetPosition(0.f, 7.f, 0.f, 0.f);
    // Set the light values
    lights_[i].SetDiffuseColour(1.f, 1.f, 1.f, 1.f);
    lights_[i].SetSpecularColour(1.f, 1.f, 1.f, 1.f);
    lights_[i].SetSpecularPower(4.f);
    lights_[i].SetAttenuation(0.95f, 0.04f, 0.f);
    lights_[i].SetRange(300.f);
    lights_[i].set_active(false);
    // Set ambient for one light only 
    if (i == 0) {
      //lights_[i].SetAmbientColour(0.3f, 0.3f, 0.3f, 1.f);
      lights_[i].SetAmbientColour(0.1f, 0.1f, 0.1f, 1.f);
      lights_[i].SetPosition(0.f, 10.f, 0.f, 0.f);
      lights_[i].SetDiffuseColour(1.f, 1.f, 1.f, 1.f);
      lights_[i].SetDirection(0.f, -1.f, -1.f);
      //lights_[i].set_spot_cutoff(45.f);
      //lights_[i].set_spot_exponent(5.f);
    lights_[i].set_active(true);

    }
    if (i == 1) {
      lights_[i].SetAmbientColour(0.1f, 0.1f, 0.1f, 1.f);
      lights_[i].SetPosition(40.f, 20.f, -1.f, 0.f);
      lights_[i].SetDiffuseColour(0.f, 1.f, 0.f, 1.f);
      lights_[i].SetDirection(0.f, -1.f, 0.f);
      lights_[i].set_active(true);
    }
    if (i == 2) {
      lights_[i].SetAmbientColour(0.1f, 0.1f, 0.1f, 1.f);
      lights_[i].SetPosition(-10.f, 50.f, 0.f, 0.f);
      lights_[i].SetDiffuseColour(1.f, 1.f, 1.f, 1.f);
      lights_[i].SetDirection(0.f, -1.f, 0.f);
      lights_[i].set_active(true);
      lights_[i].SetAttenuation(0.5f, 0.03f, 0.f);
    }
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


  // Enable alpha blending
  m_Direct3D->TurnOnAlphaBlending();
  //lights_[1].SetPosition(3.f, 5.f, 0.f, 1.f);

  // Create the first secondary render target
  render_target_0_ = new RenderTexture(m_Direct3D->GetDevice(), screenWidth,
    screenHeight, SCREEN_NEAR, SCREEN_DEPTH, L"target_0");

  // Create the ortho mesh
  ortho_mesh_0_ = new OrthoMesh(m_Direct3D->GetDevice(), screenWidth / 4, screenHeight / 4, -300, 225);

}


Lab3::~Lab3() {
  if (ortho_mesh_0_ != nullptr) {
    delete ortho_mesh_0_;
    ortho_mesh_0_ = nullptr;
  }

  if (render_target_0_ != nullptr) {
    delete render_target_0_;
    render_target_0_ = nullptr;
  }

	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (m_Mesh)
	{
		delete m_Mesh;
		m_Mesh = 0;
	}

	if (m_Shader)
	{
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


bool Lab3::Frame()
{
	bool result;

	result = BaseApplication::Frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = Render();
	if (!result)
	{
		return false;
	}

  // Increment the time counter
  prev_time_ += m_Timer->GetTime();

	return true;
}

bool Lab3::Render()
{
  RenderToTexture();

  RenderScene();

  // Present the rendered scene to the screen.
  m_Direct3D->EndScene();
	
  return true;
}

void Lab3::RenderToTexture() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
  
  // Set the render target to be the render to texture.
  render_target_0_->SetRenderTarget(m_Direct3D->GetDeviceContext());

  // Clear the render to texture.
  render_target_0_->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);

  // Generate the view matrix based on the camera's position.
  m_Camera->Update();

  // Get the world, view, and projection matrices from the camera and d3d objects.
  m_Direct3D->GetWorldMatrix(worldMatrix);
  m_Camera->GetViewMatrix(viewMatrix);
  m_Direct3D->GetProjectionMatrix(projectionMatrix);

  // Create a mock material
  szgrh::Material bunny_material;
  bunny_material.diffuse_texname = L"../res/bunny.png";
  bunny_material.bump_texname = L"../res/bunny_bump.png";
  for (unsigned int i = 0; i < 3; ++i) {
    bunny_material.ambient[i] = 1.f;
    bunny_material.diffuse[i] = 1.f;
    bunny_material.specular[i] = 1.f;
    bunny_material.transmittance[i] = 0.f;
    bunny_material.emission[i] = 0.f;
  }
  bunny_material.shininess = 7.f;
  bunny_material.ior = 0.f;
  bunny_material.dissolve = 1.f;
  bunny_material.illum = 2;
  
  normal_map_shader_->SetShaderFrameParameters(m_Direct3D->GetDeviceContext(), 
    lights_, m_Camera);
  
  // Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
  cube_mesh_->SendData(m_Direct3D->GetDeviceContext());

  XMMATRIX cube_transform = XMMatrixScaling(2.f, 2.f, 2.f) * 
    XMMatrixTranslation(0.f, 0.f, 0.f);
  normal_map_shader_->SetShaderParameters(m_Direct3D->GetDeviceContext(), 
    worldMatrix,
    viewMatrix, projectionMatrix, bunny_material);
	// Render object (combination of mesh geometry and shader process
	normal_map_shader_->Render(m_Direct3D->GetDeviceContext(), 
    cube_mesh_->GetIndexCount());

  // Reset the render target back to the original back buffer and not the render to texture anymore.
  m_Direct3D->SetBackBufferRenderTarget();
}

void Lab3::RenderScene() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	
	// Clear the scene. (default blue colour)
	m_Direct3D->BeginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Update();

  // Set per-frame shader paramters
  //m_Shader->SetShaderFrameParameters(m_Direct3D->GetDeviceContext(), lights_, m_Camera);
  normal_map_shader_->SetShaderFrameParameters(m_Direct3D->GetDeviceContext(), 
    lights_, m_Camera);
  //waves_shader_->SetShaderFrameParameters(m_Direct3D->GetDeviceContext(),
  //  lights_, m_Camera, prev_time_);
  
	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Send geometry data (from mesh)
	cube_mesh_->SendData(m_Direct3D->GetDeviceContext());
  // Create a mock material
  szgrh::Material mock_material;
  mock_material.diffuse_texname = L"../res/DefaultDiffuse.png";
  mock_material.bump_texname = L"../res/DefaultNormal.png";
  for (unsigned int i = 0; i < 3; ++i) {
    mock_material.ambient[i] = 1.f;
    mock_material.diffuse[i] = 1.f;
    mock_material.specular[i] = 1.f;
    mock_material.transmittance[i] = 0.f;
    mock_material.emission[i] = 0.f;
  }
  mock_material.shininess = 7.f;
  mock_material.ior = 0.f;
  mock_material.dissolve = 1.f;
  mock_material.illum = 2;

	// Set shader parameters (matrices and texture)
  XMMATRIX cube_transform = XMMatrixScaling(2.f, 2.f, 2.f) * 
    XMMatrixTranslation(0.f, 0.f, 0.f);
  normal_map_shader_->SetShaderParameters(m_Direct3D->GetDeviceContext(), 
    worldMatrix,
    viewMatrix, projectionMatrix, mock_material);
	// Render object (combination of mesh geometry and shader process
	normal_map_shader_->Render(m_Direct3D->GetDeviceContext(), 
    cube_mesh_->GetIndexCount());

  XMMATRIX model_transform = XMMatrixScaling(0.1f, 0.1f, 0.1f) /**
    XMMatrixTranslation(10.f, -20.f, 0.f)*/;
  // Create a base shader
  BaseShader *shader = nullptr;
  // For all the meshes in the model
  std::stack<size_t> alpha_blended_meshes;
  if (model_ != nullptr) {
    for (size_t i = 0; i < model_->meshes_.size(); ++i) {
      // If the mesh is alpha blended
      if (model_->materials_[model_->meshes_[i].mat_id()].alpha_texname 
        != L"") {
        // Defer its rendering
        alpha_blended_meshes.push(i);
        continue;
      }
      model_->meshes_[i].SendData(m_Direct3D->GetDeviceContext());
      shader = sha_manager_->GetShader(
        model_->materials_[model_->meshes_[i].mat_id()].shader_name);
      if (shader == nullptr) {
        continue;
      }
      shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), 
        model_transform, viewMatrix, projectionMatrix, 
        model_->materials_[model_->meshes_[i].mat_id()]);
      // Render object (combination of mesh geometry and shader process
      shader->Render(m_Direct3D->GetDeviceContext(),
        model_->meshes_[i].GetIndexCount());
    }

    // Render alpha blended meshes
    while (alpha_blended_meshes.size() > 0) {
      size_t i = alpha_blended_meshes.top();
      alpha_blended_meshes.pop();
      model_->meshes_[i].SendData(m_Direct3D->GetDeviceContext());
      shader = sha_manager_->GetShader(
        model_->materials_[model_->meshes_[i].mat_id()].shader_name);
      if (shader == nullptr) {
        continue;
      }
      shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), model_transform,
        viewMatrix, projectionMatrix, 
        model_->materials_[model_->meshes_[i].mat_id()]);
      // Render object (combination of mesh geometry and shader process
      shader->Render(m_Direct3D->GetDeviceContext(),
        model_->meshes_[i].GetIndexCount());
    }
  }


  // To render ortho mesh
  // Turn off the Z buffer to begin all 2D rendering.
  m_Direct3D->TurnZBufferOff();

  XMMATRIX ortho_matrix, base_view_matrix;
  m_Direct3D->GetOrthoMatrix(ortho_matrix);// ortho matrix for 2D rendering
  m_Camera->GetBaseViewMatrix(base_view_matrix);

  mock_material.diffuse_texname = render_target_0_->name();
  ortho_mesh_0_->SendData(m_Direct3D->GetDeviceContext());
  texture_shader_->SetShaderParameters(m_Direct3D->GetDeviceContext(),
    worldMatrix, base_view_matrix, ortho_matrix, 
    mock_material);
  texture_shader_->Render(m_Direct3D->GetDeviceContext(),
    ortho_mesh_0_->GetIndexCount());

  m_Direct3D->TurnZBufferOn();
}