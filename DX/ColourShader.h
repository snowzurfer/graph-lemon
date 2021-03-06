// Colour shader.h
// Simple shader example.
#ifndef _COLOURSHADER_H_
#define _COLOURSHADER_H_

#include "BaseShader.h"

using namespace std;
using namespace DirectX;


class ColourShader : public BaseShader
{

public:

  ColourShader(ID3D11Device* device, HWND hwnd);
  ~ColourShader();

  void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection);
  void Render(ID3D11DeviceContext* deviceContext,
    size_t index_count,
    size_t index_start = 0,
    size_t base_vertex = 0);

private:
  void InitShader(WCHAR*, WCHAR*);

private:
  ID3D11Buffer* m_matrixBuffer;
};

#endif