// Application.h
#ifndef _LAB2_H
#define _LAB2_H

// Includes
#include "baseapplication.h"

#include "QuadMesh.h"
#include "TextureShader.h"

class Lab2 : public BaseApplication
{
public:

  Lab2(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);
  ~Lab2();

  bool Frame();

private:
  bool Render();

private:
  TextureShader* m_Shader;
  QuadMesh* m_Mesh;

};

#endif
