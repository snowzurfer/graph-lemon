// Application.h
#ifndef _LAB3_H
#define _LAB3_H

// Includes
#include "baseapplication.h"

#include "QuadMesh.h"
#include "TextureShader.h"

class Lab3 : public BaseApplication
{
public:

	Lab3(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);
	~Lab3();

	bool Frame();

private:
	bool Render();

private:
	TextureShader* m_Shader;
	QuadMesh* m_Mesh;

};

#endif
