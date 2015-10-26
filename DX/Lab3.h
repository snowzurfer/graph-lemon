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

class Lab3 : public BaseApplication
{
public:

	Lab3(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);
	~Lab3();

	bool Frame();

private:
	bool Render();

  // Render the scene to a texture target
  void RenderToTexture();

  // Render scene normally, plus rendered target
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
  // The other render target
  RenderTexture *render_target_0_;
  // Ortho mesh onto which to render the secondary render target
  OrthoMesh *ortho_mesh_0_;

  // Used to count time
  float prev_time_;
};

#endif
