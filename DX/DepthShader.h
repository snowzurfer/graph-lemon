// Colour shader.h
// Simple shader example.
#ifndef _DEPTHSHADER_H
#define _DEPTHSHADER_H

#include "BaseShader.h"

using namespace std;
using namespace DirectX;

namespace sz {
  class ConstBufManager;
  class Material;
}

class DepthShader : public BaseShader {
public:
  // Ctor
  DepthShader(ID3D11Device* device, HWND hwnd,
    sz::ConstBufManager &buf_man);

  // Dtor
  ~DepthShader();

  void SetShaderParameters(ID3D11DeviceContext* deviceContext, 
    const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection,
    const sz::Material &mat);

  void Render(ID3D11DeviceContext* deviceContext,
    size_t index_count,
    size_t index_start = 0,
    size_t base_vertex = 0);

private:
  void InitShader(sz::ConstBufManager &buf_man, WCHAR*, WCHAR*);

private:
  ID3D11Buffer* m_matrixBuffer;

}; // class DepthShader

#endif