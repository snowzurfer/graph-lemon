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

class Lab3 : public BaseApplication
{
public:

	Lab3(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);
	~Lab3();

	bool Frame();

private:
	bool Render();

private:
	LightShader* m_Shader;
  Model *model_;
	SphereMesh* m_Mesh;
  CubeMesh *cube_mesh_;
  std::vector<Light> lights_;

};

#endif
