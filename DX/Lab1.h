// Application.h
#ifndef _LAB1_H
#define _LAB1_H

// Includes
#include "baseapplication.h"

#include "QuadMesh.h"
#include "ColourShader.h"

class Lab1 : public BaseApplication
{
public:

  Lab1(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);
  ~Lab1();

  bool Frame();

private:
  bool Render();

private:
  ColourShader* m_ColourShader;
  QuadMesh* m_Mesh;

};

#endif