#ifndef _GEOMETRYBOXSHADER_H_
#define _GEOMETRYBOXSHADER_H_

#include "BaseShader.h"

using namespace std;
using namespace DirectX;

namespace sz {
  class ConstBufManager;
  class Material;
}

class GeometryBoxShader : public BaseShader {
public:

  GeometryBoxShader(ID3D11Device* device, HWND hwnd,
    sz::ConstBufManager &buf_man);
  ~GeometryBoxShader();

  void SetShaderParameters(ID3D11DeviceContext* deviceContext,
    const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix,
    const XMMATRIX &projectionMatrix, const sz::Material &mat);
  void Render(ID3D11DeviceContext* deviceContext,
    size_t index_count,
    size_t index_start,
    size_t base_vertex);

private:
  void InitShader(WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename,
    sz::ConstBufManager &buf_man);

private:
  ID3D11Buffer* m_matrixBuffer;
};

#endif