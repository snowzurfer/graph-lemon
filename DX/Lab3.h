// Application.h
#ifndef _LAB3_H
#define _LAB3_H

// Includes
#include "baseapplication.h"

#include "SphereMesh.h"
#include "CubeMesh.h"
#include "LightShader.h"
#include "Light.h"
#include <vector>
#include "Model.h"
#include "buffer_resource_manager.h"
#include "waves_vertex_deform_shaderh.h"
#include "normal_mapping_shader.h"
#include "TextureShader.h"
#include "shader_resource_manager.h"
#include "RenderTexture.h"
#include "OrthoMesh.h"
#include "gauss_blur_h_shader.h"
#include "gauss_blur_v_shader.h"

class Lab3 : public BaseApplication
{
public:

  Lab3(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight,
    Input*);
  ~Lab3();

  bool Frame();

private:
  bool Render();

  // Render the scene to a texture target
  void RenderToTexture();

  // Render the scene to a smaller render target, hence downsampling
  void DownSample();
  
  // Render the smaller target to a bigger render target, hence upsampling
  void UpSample();

  // Perform horizontal blurring for Gaussian blur
  void HorizontalBlur();

  // Perform horizontal blurring for Gaussian blur
  void VerticalBlur();

  // Render to the back buffer from the upsampled render target
  void RenderToBackBuffer();

  // Render scene normally
  void RenderScene();

private:
  LightShader* m_Shader;
  TextureShader *texture_shader_;
  Model *model_;
  SphereMesh* m_Mesh;
  CubeMesh *cube_mesh_;
  std::vector<Light> lights_;
  szgrh::ConstBufManager *buf_manager_;
  szgrh::ShaderManager *sha_manager_;
  WavesVertexDeformShader *waves_shader_;
  NormalMappingShader *normal_map_shader_;
  GaussBlurHShader *gauss_blur_h_shader_;
  GaussBlurVShader *gauss_blur_v_shader_;
  // The other render targets
  RenderTexture *render_target_downsample0_;
  RenderTexture *render_target_downsample1_;
  RenderTexture *render_target_upsample_;
  // Ortho meshes for post processing
  OrthoMesh *ortho_mesh_downsample_;
  OrthoMesh *ortho_mesh_upsample_;

  // Size of the screen
  unsigned int screen_width_, screen_height_;

  // Used to count time
  float prev_time_;
};

#endif